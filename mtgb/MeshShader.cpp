#include "MeshShader.h"
#include "CameraSystem.h"

namespace
{
	mtgb::Vector4 lightDir { 0, 1, 1, 0 };
}

void mtgb::MeshShader::Initialize(ID3D11Device* _pDevice)
{
	InitializeCommonGpuResources(_pDevice, L"Shader/FbxParts.hlsl");

	CD3D11_RASTERIZER_DESC cRasterizerDesc = CD3D11_RASTERIZER_DESC(D3D11_RASTERIZER_DESC {
		.FillMode			   = D3D11_FILL_SOLID, // 塗りつぶし: solid
		.CullMode			   = D3D11_CULL_BACK,  // カリング: 陰面消去
		.FrontCounterClockwise = FALSE,			   // 三角形の正面向き = 時計回り
		.DepthBias			   = {},
		.DepthBiasClamp		   = {},
		.SlopeScaledDepthBias  = {},
		.DepthClipEnable	   = true, // クリッピングを有効にする
		.ScissorEnable		   = {},
		.MultisampleEnable	   = {},
		.AntialiasedLineEnable = {},
	});
	_pDevice->CreateRasterizerState(&cRasterizerDesc, pRasterizerState.ReleaseAndGetAddressOf());
}

void mtgb::MeshShader::Draw(ID3D11DeviceContext* _pCtx, const Transform& _transform, MeshAsset* _pAsset, int _frame)
{
	using namespace DirectX;
	DirectX11Draw::SetIsWriteToDepthBuffer(true);
	// 描画情報をシェーダに渡す
	UINT stride { sizeof(Vertex) };
	UINT offset { 0 };
	_pCtx->IASetVertexBuffers(0, 1, _pAsset->pVertexBuffer.GetAddressOf(), &stride, &offset);

	// 使用するコンスタントバッファをシェーダに伝える
	auto itr = cBufferMap_.find("global");
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
		D3D11_MAPPED_SUBRESOURCE pdata_;
		ConstantBuffer cb {};
		Matrix4x4 mWorld {};
		_transform.GenerateWorldMatrix(&mWorld);

		Matrix4x4 mView {}; // ビュー行列
		CAMERA.GetViewMatrix(&mView);

		Matrix4x4 mProj {}; // プロジェクション行列
		CAMERA.GetProjMatrix(&mProj);

		cb.g_matrixWorldViewProj = XMMatrixTranspose(mWorld * mView * mProj);
		cb.g_matrixWorld		 = XMMatrixTranspose(mWorld);

		XMMATRIX matRotate_, rotateX_, rotateY_, rotateZ_;
		rotateX_   = XMMatrixRotationX(XMConvertToRadians(_transform.rotate.f[0]));
		rotateY_   = XMMatrixRotationY(XMConvertToRadians(_transform.rotate.f[1]));
		rotateZ_   = XMMatrixRotationZ(XMConvertToRadians(_transform.rotate.f[2]));
		matRotate_ = rotateZ_ * rotateY_ * rotateX_;

		XMMATRIX matScale_ = XMMatrixScaling(_transform.scale.x, _transform.scale.y, _transform.scale.z);

		cb.g_matrixNormalTrans = XMMatrixTranspose(matRotate_ * XMMatrixInverse(nullptr, matScale_));
		cb.g_ambient		   = _pAsset->materials[i].ambient;
		cb.g_diffuse		   = _pAsset->materials[i].diffuse;
		cb.g_speculer		   = _pAsset->materials[i].specular;
		cb.g_shininess		   = _pAsset->materials[i].shininess;
		CAMERA.GetPosition(&cb.g_cameraPosition);

		cb.g_lightDirection = lightDir; // ライトの向き
		cb.g_isTexture		= (_pAsset->materials[i].pTexture != nullptr);
		cb.g_textureScale	= Vector4(_transform.scale.x, _transform.scale.y, _transform.scale.z, 0.0f);

		cBuffer.SetConstantBuffer(cb);
		cBuffer.ApplyChanges(_pCtx);

		if (cb.g_isTexture)
		{
			ID3D11SamplerState* pSampler = _pAsset->materials[i].pTexture->GetSamplerState();
			_pCtx->PSSetSamplers(0, 1, &pSampler);

			ID3D11ShaderResourceView* pShaderResourceView = _pAsset->materials[i].pTexture->GetShaderResourceView();
			_pCtx->PSSetShaderResources(0, 1, &pShaderResourceView);
		}
		else
		{
			_pCtx->PSSetSamplers(0, 1, DirectX11Draw::pDefaultSamplerState_.GetAddressOf());

			ID3D11ShaderResourceView* pNullSRV = nullptr;
			_pCtx->PSSetShaderResources(0, 1, &pNullSRV);
		}

		// ポリゴンメッシュを描画する
		_pCtx->DrawIndexed(_pAsset->materials[i].polygonCount * 3, 0, 0);
	}
}