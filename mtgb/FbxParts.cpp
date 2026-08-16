#include "FbxParts.h"
#include <DirectXMath.h>
#include <d3d11.h>
#include "ReleaseUtility.h"
#include "MTAssert.h"
#include "Transform.h"
#include "Debug.h"
#include "MTStringUtility.h"
#include "DirectX11Draw.h"
#include "Game.h"
#include "CameraSystem.h"
#include <cmath>
#include <algorithm>

namespace
{
	mtgb::Vector4 lightDir { 0, 1, 1, 0 };
}

mtgb::FbxParts::FbxParts(FbxNode* _parent, double _unitScaleFactor)
	: pNode_ { _parent }
	, unitScaleFactor_ { _unitScaleFactor }
{
	pMeshAsset_ = MeshAsset::LoadFromFbx(_parent, _unitScaleFactor);
	pMeshAsset_->CreateGpuResources(DirectX11Draw::pDevice_.Get());
}

mtgb::FbxParts::~FbxParts() {}

void mtgb::FbxParts::Draw(const Transform& _transform)
{
	using namespace DirectX;

	if (pMeshAsset_->hasSkinnedMesh)
	{
		DirectX11Draw::SetShader(ShaderType::FBX_PARTS_SKIN);
	}
	DirectX11Draw::SetIsWriteToDepthBuffer(true);
	// 描画情報をシェーダに渡す
	UINT stride { sizeof(Vertex) };
	UINT offset { 0 };
	DirectX11Draw::pContext_->IASetVertexBuffers(0, 1, pMeshAsset_->pVertexBuffer.GetAddressOf(), &stride, &offset);

	// 使用するコンスタントバッファをシェーダに伝える
	DirectX11Draw::pContext_->VSSetConstantBuffers(0, 1, pConstantBuffer_.GetAddressOf());
	DirectX11Draw::pContext_->PSSetConstantBuffers(0, 1, pConstantBuffer_.GetAddressOf());

	if (pMeshAsset_->hasSkinnedMesh)
	{
		SetBoneMatrix(); // ボーン行列
		DirectX11Draw::pContext_->VSSetConstantBuffers(1, 1, pMeshAsset_->pBoneConstantBuffer.GetAddressOf());
	}

	// カメラシステムへのアクセス用
	const CameraSystem& CAMERA { Game::System<CameraSystem>() };

	// シェーダのコンスタントバッファーに各種データを渡す
	for (DWORD i = 0; i < pMeshAsset_->materialCount; i++)
	{
		DirectX11Draw::pContext_->IASetIndexBuffer(pMeshAsset_->ppIndexBuffer[i].Get(), DXGI_FORMAT_R32_UINT, 0);

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
		cb.g_ambient		   = pMeshAsset_->materials[i].ambient;
		cb.g_diffuse		   = pMeshAsset_->materials[i].diffuse;
		cb.g_speculer		   = pMeshAsset_->materials[i].specular;
		cb.g_shininess		   = pMeshAsset_->materials[i].shininess;
		CAMERA.GetPosition(&cb.g_cameraPosition);

		cb.g_lightDirection = lightDir; // ライトの向き
		cb.g_isTexture		= (pMeshAsset_->materials[i].pTexture != nullptr);
		cb.g_textureScale	= Vector4(_transform.scale.x, _transform.scale.y, _transform.scale.z, 0.0f);
		DirectX11Draw::pContext_->Map(pConstantBuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata_);
		memcpy_s(pdata_.pData, pdata_.RowPitch, (void*)(&cb), sizeof(cb));

		if (cb.g_isTexture)
		{
			ID3D11SamplerState* pSampler = pMeshAsset_->materials[i].pTexture->GetSamplerState();
			DirectX11Draw::pContext_->PSSetSamplers(0, 1, &pSampler);

			ID3D11ShaderResourceView* pShaderResourceView = pMeshAsset_->materials[i].pTexture->GetShaderResourceView();
			DirectX11Draw::pContext_->PSSetShaderResources(0, 1, &pShaderResourceView);
		}
		else
		{
			DirectX11Draw::pContext_->PSSetSamplers(0, 1, DirectX11Draw::pDefaultSamplerState_.GetAddressOf());

			ID3D11ShaderResourceView* pNullSRV = nullptr;
			DirectX11Draw::pContext_->PSSetShaderResources(0, 1, &pNullSRV);
		}
		DirectX11Draw::pContext_->Unmap(pConstantBuffer_.Get(), 0);

		// ポリゴンメッシュを描画する
		DirectX11Draw::pContext_->DrawIndexed(pMeshAsset_->materials[i].polygonCount * 3, 0, 0);
	}
}

void mtgb::FbxParts::DrawSkinAnimation(const Transform& _transform, FbxTime _time)
{
	// アニメーション時間を設定
	SetAnimationTime(_time);

	// 通常の描画処理を呼び出し
	Draw(_transform);
}

void mtgb::FbxParts::DrawMeshAnimation(const Transform& _transform, FbxTime _time)
{
	Draw(_transform);
}

bool mtgb::FbxParts::TryGetBonePosition(std::string_view _boneName, Vector3* _pPosition)
{
	for (int i = 0; i < pMeshAsset_->boneCount; i++)
	{
		// 見つかった！
		if (_boneName == pMeshAsset_->ppCluster[i]->GetLink()->GetName())
		{
			FbxAMatrix m {};
			pMeshAsset_->ppCluster[i]->GetTransformLinkMatrix(m);

			_pPosition->x = static_cast<float>(m[3][0]);
			_pPosition->y = static_cast<float>(m[3][1]);
			_pPosition->z = static_cast<float>(m[3][2]);

			return true;
		}
	}

	return false;
}

bool mtgb::FbxParts::TryGetBonePositionAtNow(std::string_view _boneName, Vector3* _pPosition)
{
	auto itr { pMeshAsset_->boneNamePair.find(_boneName) };
	if (itr != pMeshAsset_->boneNamePair.end()) // end じゃないなら見つかった
	{
		Matrix4x4 m {};
		_pPosition->x = DirectX::XMVectorGetX(m.r[3]);
		_pPosition->y = DirectX::XMVectorGetY(m.r[3]);
		_pPosition->z = DirectX::XMVectorGetZ(m.r[3]);

		return true;
	}
	return false;
}

void mtgb::FbxParts::SetBoneMatrix()
{
	using namespace DirectX;

	BoneMatrices boneMatrices_;
	for (int i = 0; i < pMeshAsset_->boneCount; i++)
	{
		FbxAnimEvaluator* evaluator { pMeshAsset_->ppCluster[i]->GetLink()->GetScene()->GetAnimationEvaluator() };
		FbxMatrix mCurrent { evaluator->GetNodeGlobalTransform(pMeshAsset_->ppCluster[i]->GetLink(), currentTime_) };

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
		Matrix4x4 bindPoseInv = XMMatrixInverse(nullptr, pMeshAsset_->bones[i].bindPose);

		// 最終的なボーン行列 = バインドポーズの逆行列 × 現在のポーズ
		Matrix4x4 finalBoneMatrix = bindPoseInv * currentPose;

		boneMatrices_.boneMatrices[i] = XMMatrixTranspose(finalBoneMatrix);
	}

	// コンスタントバッファに書き込み
	D3D11_MAPPED_SUBRESOURCE mappedSubResource {};
	DirectX11Draw::pContext_
		->Map(pMeshAsset_->pBoneConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);

	memcpy_s(mappedSubResource.pData, mappedSubResource.RowPitch, (void*)&boneMatrices_, sizeof(BoneMatrices));
	DirectX11Draw::pContext_->Unmap(pMeshAsset_->pBoneConstantBuffer.Get(), 0);
}

void mtgb::FbxParts::SetAnimationTime(const FbxTime& _time)
{
	currentTime_ = _time;
}

void mtgb::FbxParts::InitializeVertexBuffer(ID3D11Device* _pDevice) {}

void mtgb::FbxParts::InitializeIndexBuffer(ID3D11Device* _pDevice) {}

void mtgb::FbxParts::InitializeConstantBuffer(ID3D11Device* _pDevice)
{
	const D3D11_BUFFER_DESC BUFFER_DESC {
		.ByteWidth			 = sizeof(ConstantBuffer),
		.Usage				 = D3D11_USAGE_DYNAMIC, // MEMO: 途中で書き換えるためdynamic
		.BindFlags			 = D3D11_BIND_CONSTANT_BUFFER,
		.CPUAccessFlags		 = D3D11_CPU_ACCESS_WRITE,
		.MiscFlags			 = 0,
		.StructureByteStride = 0,
	};

	HRESULT hResult {};
	hResult = _pDevice->CreateBuffer(
		&BUFFER_DESC,
		nullptr, // 初期データなし
		pConstantBuffer_.ReleaseAndGetAddressOf()
	);

	massert(SUCCEEDED(hResult) && "コンスタントバッファの作成に失敗 @FbxParts::InitializeConstantBuffer");
}
