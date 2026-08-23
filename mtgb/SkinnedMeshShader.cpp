#include "SkinnedMeshShader.h"
#include "CameraSystem.h"
#include "CompileShaderUtility.h"
#include "3DCommonConstantBuffer.h"

namespace
{
	mtgb::Vector4 lightDir { 0, 1, 1, 0 };
}

void SkinnedMeshShader::Initialize(ID3D11Device* _pDevice)
{
	InitializeCommonGpuResources(_pDevice, L"Shader/FbxPartsSkin.hlsl");

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
	_pDevice->CreateRasterizerState(&cRasterizerDesc, pRasterizerState_.ReleaseAndGetAddressOf());
}

void SkinnedMeshShader::Draw(ID3D11DeviceContext* _pCtx, const Transform& _transform, MeshAsset* _pAsset, int _frame)
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

	if (_pAsset->hasSkinnedMesh)
	{
		auto itr = cBufferMap_.find("BoneMatrices");
		if (itr == cBufferMap_.end())
		{
			assert(false);
			return;
		}
		auto& boneCBuffer = itr->second;
		boneCBuffer.BindVS(_pCtx);
		SetBoneMatrix(_pCtx, _pAsset, _frame); // ボーン行列
	}

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

void SkinnedMeshShader::SetBoneMatrix(ID3D11DeviceContext* _pCtx, MeshAsset* _pAsset, int _frame)
{
	using namespace DirectX;
	auto frameRate = _pAsset->pFbxNode->GetScene()->GetGlobalSettings().GetTimeMode();
	BoneMatrices boneMatrices_;
	FbxTime time;
	time.SetTime(0, 0, 0, _frame, 0, 0, frameRate);
	for (int i = 0; i < _pAsset->boneCount; i++)
	{
		FbxAnimEvaluator* evaluator { _pAsset->ppCluster[i]->GetLink()->GetScene()->GetAnimationEvaluator() };
		FbxMatrix mCurrent { evaluator->GetNodeGlobalTransform(_pAsset->ppCluster[i]->GetLink(), time) };

		// FbxMatrix を DirectX::XMMATRIX に変換
		XMFLOAT4X4 pose {};
		for (DWORD x = 0; x < 4; x++)
		{
			for (DWORD y = 0; y < 4; y++)
			{
				pose(x, y) = static_cast<float>(mCurrent.Get(x, y));
			}
		}
		Matrix4x4 currentPose = XMLoadFloat4x4(&pose);

		// バインドポーズの逆行列
		Matrix4x4 bindPoseInv = XMMatrixInverse(nullptr, _pAsset->bones[i].bindPose);

		// 最終的なボーン行列 = バインドポーズの逆行列 × 現在のポーズ
		Matrix4x4 finalBoneMatrix = bindPoseInv * currentPose;

		boneMatrices_.boneMatrices[i] = XMMatrixTranspose(finalBoneMatrix);
	}

	auto itr = cBufferMap_.find("BoneMatrices");
	if (itr == cBufferMap_.end())
	{
		assert(false);
		return;
	}
	auto& boneCBuffer = itr->second;
	boneCBuffer.SetConstantBuffer(boneMatrices_);
	boneCBuffer.ApplyChanges(_pCtx);
}
