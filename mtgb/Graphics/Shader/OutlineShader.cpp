#include "OutlineShader.h"
#include "CameraSystem.h"
mtgb::OutlineShader::OutlineShader()
	: outlineColor_ { 1.0f, 0.6f, 0.0f, 1.0f }
	, outlineScale_ { 1.06f }
{
}
void mtgb::OutlineShader::Initialize(ID3D11Device* _pDevice)
{
	InitializeCommonGpuResources(_pDevice, L"Shader/Outline.hlsl");

	CD3D11_RASTERIZER_DESC cRasterizerDesc = CD3D11_RASTERIZER_DESC(D3D11_RASTERIZER_DESC {
		.FillMode			   = D3D11_FILL_SOLID, // 塗りつぶし: solid
		.CullMode			   = D3D11_CULL_FRONT, // カリング: 陰面消去
		.FrontCounterClockwise = FALSE,			   // 三角形の正面向き = 時計回り
		.DepthBias			   = {},
		.DepthBiasClamp		   = {},
		.SlopeScaledDepthBias  = {},
		.DepthClipEnable	   = true, // クリッピングを有効にする
		.ScissorEnable		   = {},
		.MultisampleEnable	   = {},
		.AntialiasedLineEnable = {},
	});
	_pDevice->CreateRasterizerState(&cRasterizerDesc, pRasterizerState_.ReleaseAndGetAddressOf());
}

void mtgb::OutlineShader::Draw(ID3D11DeviceContext* _pCtx, const Transform& _transform, MeshAsset* _pAsset, int _frame)
{
	using namespace DirectX;
	DirectX11Draw::SetIsWriteToDepthBuffer(true);
	// 描画情報をシェーダに渡す
	UINT stride { sizeof(Vertex) };
	UINT offset { 0 };
	_pCtx->IASetVertexBuffers(0, 1, _pAsset->pVertexBuffer.GetAddressOf(), &stride, &offset);

	// 使用するコンスタントバッファをシェーダに伝える
	auto itr = cBufferMap_.find("outline");
	if (itr == cBufferMap_.end())
	{
		assert(false);
		return;
	}
	auto& cBuffer = itr->second;
	cBuffer.BindVS(_pCtx);
	cBuffer.BindPS(_pCtx);

	// カメラシステムへのアクセス用
	const CameraSystem& CAMERA { Game::System<CameraSystem>() };

	// シェーダのコンスタントバッファーに各種データを渡す
	for (DWORD i = 0; i < _pAsset->materialCount; i++)
	{
		_pCtx->IASetIndexBuffer(_pAsset->ppIndexBuffer[i].Get(), DXGI_FORMAT_R32_UINT, 0);

		// パラメータの受け渡し
		ConstantBuffer cb {};
		Matrix4x4 mWorld {};
		_transform.GenerateWorldMatrix(&mWorld);

		Matrix4x4 mView {}; // ビュー行列
		CAMERA.GetViewMatrix(&mView);

		Matrix4x4 mProj {}; // プロジェクション行列
		CAMERA.GetProjMatrix(&mProj);

		cb.g_matrixWorldViewProj = XMMatrixTranspose(mWorld * mView * mProj);
		cb.g_color				 = outlineColor_;
		cb.g_scale				 = outlineScale_;

		cBuffer.SetConstantBuffer(cb);
		cBuffer.ApplyChanges(_pCtx);

		// ポリゴンメッシュを描画する
		_pCtx->DrawIndexed(_pAsset->materials[i].polygonCount * 3, 0, 0);
	}
}
