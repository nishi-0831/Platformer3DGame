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
	Vector4 lightDir { 0, 1, 1, 0 };
}

// FbxParts コンストラクタの初期化リストを拡張して、全メンバー変数を初期化
mtgb::FbxParts::FbxParts(FbxNode* _parent, double _unitScaleFactor)
	: hasSkinnedMesh_ { false }
	, vertexCount_ { 0 }
	, polygonCount_ { 0 }
	, indexCount_ { 0 }
	, materialCount_ { 0 }
	, polygonVertexCount_ { 0 }
	, pNode_ { nullptr }
	, pMaterial_ { nullptr }
	, pMesh_ { nullptr }
	, pSkin_ { nullptr }
	, ppCluster_ { nullptr }
	, boneCount_ { 0 }
	, pVertexes_ { nullptr }
	, ppIndexData_ { nullptr }
	, ppIndexBuffer_ { nullptr }
	, unitScaleFactor_ { _unitScaleFactor }
	, fbxToWorldScaleFactor_ { static_cast<float>(1.0f / _unitScaleFactor) }
{
	if (_parent != nullptr)
	{
		pNode_ = _parent;
		pMesh_ = _parent->GetMesh();
	}

	massert(pMesh_ != nullptr && "FbxParts: pMesh_ is null");
}

mtgb::FbxParts::~FbxParts() {}

void mtgb::FbxParts::Initialize()
{
	// MEMO: テクスチャUVが異なる頂点を分割&複製 → UV情報と頂点情報の一致調整
	// pMesh_->SplitPoints(FbxLayerElement::eTextureDiffuse);
	// 各情報の個数を取得
	vertexCount_		= pMesh_->GetControlPointsCount(); // 頂点の数
	polygonCount_		= pMesh_->GetPolygonCount();	   // ポリゴンの数
	polygonVertexCount_ = pMesh_->GetPolygonVertexCount(); // ポリゴン頂点インデックス数
	InitializeMaterial();
	IShader::Initialize(); // 頂点・インデックス・定数 バッファの初期化
}

void mtgb::FbxParts::Release()
{
	SAFE_DELETE_ARRAY(ppCluster_);

	SAFE_DELETE_ARRAY(pVertexes_);
	for (DWORD i = 0; i < materialCount_; i++)
	{
		ppIndexBuffer_[i].Reset();
		SAFE_DELETE(ppIndexData_[i]);
		SAFE_DELETE(pMaterial_[i].pTexture);
	}
	for (auto& indexBuffer : ppIndexBuffer_)
	{
		indexBuffer.Reset();
	}
	SAFE_DELETE_ARRAY(ppIndexData_);
	SAFE_DELETE_ARRAY(pMaterial_);

	pVertexBuffer_.Reset();
	pConstantBuffer_.Reset();
}

void mtgb::FbxParts::Draw(const Transform& _transform)
{
	using namespace DirectX;

	if (hasSkinnedMesh_)
	{
		DirectX11Draw::SetShader(ShaderType::FBX_PARTS_SKIN);
	}
	DirectX11Draw::SetIsWriteToDepthBuffer(true);
	// 描画情報をシェーダに渡す
	UINT stride { sizeof(Vertex) };
	UINT offset { 0 };
	DirectX11Draw::pContext_->IASetVertexBuffers(0, 1, pVertexBuffer_.GetAddressOf(), &stride, &offset);

	// 使用するコンスタントバッファをシェーダに伝える
	DirectX11Draw::pContext_->VSSetConstantBuffers(0, 1, pConstantBuffer_.GetAddressOf());
	DirectX11Draw::pContext_->PSSetConstantBuffers(0, 1, pConstantBuffer_.GetAddressOf());

	if (hasSkinnedMesh_)
	{
		SetBoneMatrix(); // ボーン行列
		DirectX11Draw::pContext_->VSSetConstantBuffers(1, 1, pBoneConstantBuffer_.GetAddressOf());
	}

	// カメラシステムへのアクセス用
	const CameraSystem& CAMERA { Game::System<CameraSystem>() };

	// シェーダのコンスタントバッファーに各種データを渡す
	for (DWORD i = 0; i < materialCount_; i++)
	{
		UINT stride { sizeof(int) };
		UINT offset { 0 };
		DirectX11Draw::pContext_->IASetIndexBuffer(ppIndexBuffer_[i].Get(), DXGI_FORMAT_R32_UINT, 0);

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
		cb.g_ambient		   = pMaterial_[i].ambient;
		cb.g_diffuse		   = pMaterial_[i].diffuse;
		cb.g_speculer		   = pMaterial_[i].specular;
		cb.g_shininess		   = pMaterial_[i].shininess;
		CAMERA.GetPosition(&cb.g_cameraPosition);

		cb.g_lightDirection = lightDir; // ライトの向き
		cb.g_isTexture		= (pMaterial_[i].pTexture != nullptr);
		cb.g_textureScale	= Vector4(_transform.scale.x, _transform.scale.y, _transform.scale.z, 0.0f);
		DirectX11Draw::pContext_->Map(pConstantBuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata_);
		memcpy_s(pdata_.pData, pdata_.RowPitch, (void*)(&cb), sizeof(cb));

		if (cb.g_isTexture)
		{
			ID3D11SamplerState* pSampler = pMaterial_[i].pTexture->GetSamplerState();
			DirectX11Draw::pContext_->PSSetSamplers(0, 1, &pSampler);

			ID3D11ShaderResourceView* pShaderResourceView = pMaterial_[i].pTexture->GetShaderResourceView();
			DirectX11Draw::pContext_->PSSetShaderResources(0, 1, &pShaderResourceView);
		}
		else
		{
			// テクスチャがない場合でもデフォルトサンプラーを設定
			ID3D11SamplerState* pDefaultSampler = nullptr;
			DirectX11Draw::pContext_->PSSetSamplers(0, 1, DirectX11Draw::pDefaultSamplerState_.GetAddressOf());

			ID3D11ShaderResourceView* pNullSRV = nullptr;
			DirectX11Draw::pContext_->PSSetShaderResources(0, 1, &pNullSRV);
		}
		DirectX11Draw::pContext_->Unmap(pConstantBuffer_.Get(), 0);

		// ポリゴンメッシュを描画する
		DirectX11Draw::pContext_->DrawIndexed(pMaterial_[i].polygonCount * 3, 0, 0);
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

bool mtgb::FbxParts::TryGetBonePosition(const std::string& _boneName, Vector3* _pPosition)
{
	for (int i = 0; i < boneCount_; i++)
	{
		// 見つかった！
		if (_boneName == ppCluster_[i]->GetLink()->GetName())
		{
			FbxAMatrix m {};
			ppCluster_[i]->GetTransformLinkMatrix(m);

			_pPosition->x = static_cast<float>(m[3][0]);
			_pPosition->y = static_cast<float>(m[3][1]);
			_pPosition->z = static_cast<float>(m[3][2]);

			return true;
		}
	}

	return false;
}

bool mtgb::FbxParts::TryGetBonePositionAtNow(const std::string& _boneName, Vector3* _pPosition)
{
	auto itr { boneNamePair_.find(_boneName) };
	if (itr != boneNamePair_.end()) // end じゃないなら見つかった
	{
		Matrix4x4 m {};
		_pPosition->x = DirectX::XMVectorGetX(m.r[3]);
		_pPosition->y = DirectX::XMVectorGetY(m.r[3]);
		_pPosition->z = DirectX::XMVectorGetZ(m.r[3]);

		return true;
	}
	return false;
}

void mtgb::FbxParts::InitializeVertexBuffer(ID3D11Device* _pDevice)
{
	vertexCount_		   = pMesh_->GetControlPointsCount();
	pVertexes_			   = new Vertex[vertexCount_] {};
	FbxDeformer* pDeformer = pMesh_->GetDeformer(0);
	hasSkinnedMesh_		   = (pDeformer != nullptr);

	for (uint32_t poly = 0; poly < polygonCount_; poly++)
	{
		for (uint32_t vertex = 0; vertex < 3; vertex++)
		{
			int index { pMesh_->GetPolygonVertex(poly, vertex) };

			// インデックスが範囲内かチェック
			if (index < 0 || index >= static_cast<int>(vertexCount_))
			{
				massert(false && "Vertex index out of range in FbxParts::InitializeVertexBuffer");
				continue;
			}

			// 頂点の座標
			FbxVector4 position		   = pMesh_->GetControlPointAt(index);
			pVertexes_[index].position = {
				static_cast<float>(position[0]) * fbxToWorldScaleFactor_,
				static_cast<float>(position[1]) * fbxToWorldScaleFactor_,
				static_cast<float>(position[2]) * fbxToWorldScaleFactor_,

			};

			// 頂点の法線
			FbxVector4 normal;
			pMesh_->GetPolygonVertexNormal(poly, vertex, normal);
			pVertexes_[index].normal = {
				static_cast<float>(normal[0]),
				static_cast<float>(normal[1]),
				static_cast<float>(normal[2]),
			};

			// スキニング関連の初期化
			if (hasSkinnedMesh_)
			{
				for (int i = 0; i < 4; ++i)
				{
					pVertexes_[index].boneIndex[i]	= 0;
					pVertexes_[index].boneWeight[i] = 0.0f;
				}
			}
			else
			{
				// スキニング無しの場合はデフォルト値
				pVertexes_[index].boneIndex[0]	= 0;
				pVertexes_[index].boneWeight[0] = 1.0f;
				for (int i = 1; i < 4; ++i)
				{
					pVertexes_[index].boneIndex[i]	= 0;
					pVertexes_[index].boneWeight[i] = 0.0f;
				}
			}
		}
	}

	// UV座標の処理

	int uvCount { pMesh_->GetTextureUVCount() };
	FbxLayerElementUV* pUV { pMesh_->GetLayer(0)->GetUVs() };
	if (pUV == nullptr || uvCount == 0)
	{
		// UVが存在しない場合、デフォルト値を設定
		for (uint32_t i = 0; i < vertexCount_; i++)
		{
			pVertexes_[i].uv = { 0.0f, 0.0f, 0.0f };
		}
	}
	else
	{
		FbxLayerElement::EMappingMode mappingMode	  = pUV->GetMappingMode();
		FbxLayerElement::EReferenceMode referenceMode = pUV->GetReferenceMode();

		if (mappingMode == FbxLayerElement::eByControlPoint)
		{
			if (referenceMode == FbxLayerElement::eDirect)
			{
				// 直接アクセス: コントロールポイントごとに1つのUV
				int writeCount = (std::min)(uvCount, static_cast<int>(vertexCount_));
				for (int i = 0; i < writeCount; i++)
				{
					FbxVector2 uv	 = pUV->GetDirectArray().GetAt(i);
					pVertexes_[i].uv = { static_cast<float>(uv[0]),
										 static_cast<float>(1.0 - uv[1]), // Y座標を反転
										 0.0f };
				}
			}
			else if (referenceMode == FbxLayerElement::eIndexToDirect)
			{
				// インデックス参照: コントロールポイントごとにインデックス
				for (uint32_t i = 0; i < vertexCount_; i++)
				{
					if (i < static_cast<uint32_t>(pUV->GetIndexArray().GetCount()))
					{
						int uvIndex = pUV->GetIndexArray().GetAt(i);
						if (uvIndex >= 0 && uvIndex < pUV->GetDirectArray().GetCount())
						{
							FbxVector2 uv	 = pUV->GetDirectArray().GetAt(uvIndex);
							pVertexes_[i].uv = { static_cast<float>(uv[0]), static_cast<float>(1.0 - uv[1]), 0.0f };
						}
					}
				}
			}
		}
		else if (mappingMode == FbxLayerElement::eByPolygonVertex)
		{
			if (referenceMode == FbxLayerElement::eDirect)
			{
				// 直接アクセス: ポリゴン頂点ごとに1つのUV
				int polygonVertexIndex = 0;
				for (uint32_t poly = 0; poly < polygonCount_; poly++)
				{
					for (uint32_t vertex = 0; vertex < 3; vertex++)
					{
						int controlPointIndex = pMesh_->GetPolygonVertex(poly, vertex);

						if (polygonVertexIndex < pUV->GetDirectArray().GetCount() && controlPointIndex >= 0 &&
							controlPointIndex < static_cast<int>(vertexCount_))
						{

							FbxVector2 uv					 = pUV->GetDirectArray().GetAt(polygonVertexIndex);
							pVertexes_[controlPointIndex].uv = { static_cast<float>(uv[0]),
																 static_cast<float>(1.0 - uv[1]),
																 0.0f };
						}
						polygonVertexIndex++;
					}
				}
			}
			else if (referenceMode == FbxLayerElement::eIndexToDirect)
			{
				// インデックス参照: ポリゴン頂点ごとにインデックス
				int polygonVertexIndex = 0;
				for (uint32_t poly = 0; poly < polygonCount_; poly++)
				{
					for (uint32_t vertex = 0; vertex < 3; vertex++)
					{
						int controlPointIndex = pMesh_->GetPolygonVertex(poly, vertex);

						if (polygonVertexIndex < pUV->GetIndexArray().GetCount() && controlPointIndex >= 0 &&
							controlPointIndex < static_cast<int>(vertexCount_))
						{

							int uvIndex = pUV->GetIndexArray().GetAt(polygonVertexIndex);
							if (uvIndex >= 0 && uvIndex < pUV->GetDirectArray().GetCount())
							{
								FbxVector2 uv					 = pUV->GetDirectArray().GetAt(uvIndex);
								pVertexes_[controlPointIndex].uv = { static_cast<float>(uv[0]),
																	 static_cast<float>(1.0 - uv[1]),
																	 0.0f };
							}
						}
						polygonVertexIndex++;
					}
				}
			}
		}

		// 骨の処理
		if (hasSkinnedMesh_)
		{
			InitializeSkelton();
		}

		const D3D11_BUFFER_DESC BUFFER_DESC {
			.ByteWidth			 = static_cast<UINT>(sizeof(Vertex) * vertexCount_),
			.Usage				 = D3D11_USAGE_DYNAMIC, // MEMO: 途中で書き換えるため dynamic
			.BindFlags			 = D3D11_BIND_VERTEX_BUFFER,
			.CPUAccessFlags		 = D3D11_CPU_ACCESS_WRITE, // 途中で書き換えるから
			.MiscFlags			 = 0,
			.StructureByteStride = 0,
		};

		const D3D11_SUBRESOURCE_DATA INITIALIZE_DATA {
			.pSysMem		  = pVertexes_,
			.SysMemPitch	  = 0,
			.SysMemSlicePitch = 0,
		};

		_pDevice->CreateBuffer(&BUFFER_DESC, &INITIALIZE_DATA, pVertexBuffer_.ReleaseAndGetAddressOf());
	}
}

void mtgb::FbxParts::InitializeIndexBuffer(ID3D11Device* _pDevice)
{
	ppIndexBuffer_.resize(materialCount_);
	ppIndexData_ = new DWORD*[materialCount_];

	int count { 0 };
	for (DWORD i = 0; i < materialCount_; i++)
	{
		count = 0;
		DWORD* pIndex { new DWORD[polygonCount_ * 3] {} };
		for (DWORD j = 0; j < polygonCount_; j++)
		{
			FbxLayerElementMaterial* material { pMesh_->GetLayer(0)->GetMaterials() };
			int materialId { material->GetIndexArray().GetAt(j) };
			if (materialId == i)
			{
				for (DWORD k = 0; k < 3; k++)
				{
					pIndex[count + k] = pMesh_->GetPolygonVertex(j, k);
				}
				count += 3;
			}
		}

		// バッファの設定
		const D3D11_BUFFER_DESC BUFFER_DESC {
			.ByteWidth			 = static_cast<UINT>(sizeof(DWORD) * count),
			.Usage				 = D3D11_USAGE_DEFAULT, // MEMO: 途中で書き換えないため DEFAULT
			.BindFlags			 = D3D11_BIND_INDEX_BUFFER,
			.CPUAccessFlags		 = 0,
			.MiscFlags			 = 0,
			.StructureByteStride = 0,
		};

		// 初期データ
		const D3D11_SUBRESOURCE_DATA INITIALIZE_DATA {
			.pSysMem		  = pIndex,
			.SysMemPitch	  = 0,
			.SysMemSlicePitch = 0,
		};

		HRESULT hResult {};
		hResult = _pDevice->CreateBuffer(&BUFFER_DESC, &INITIALIZE_DATA, ppIndexBuffer_[i].ReleaseAndGetAddressOf());

		massert(SUCCEEDED(hResult) && "インデックスバッファの作成に失敗");

		pMaterial_[i].polygonCount = count / 3;
		ppIndexData_[i]			   = new DWORD[count];
		memcpy(ppIndexData_[i], pIndex, sizeof(DWORD) * count);
		SAFE_DELETE_ARRAY(pIndex);
	}
}

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

	// ボーン行列用コンスタントバッファ
	if (hasSkinnedMesh_)
	{
		const D3D11_BUFFER_DESC BUFFER_DESC {
			.ByteWidth			 = sizeof(BoneMatrices),
			.Usage				 = D3D11_USAGE_DYNAMIC,
			.BindFlags			 = D3D11_BIND_CONSTANT_BUFFER,
			.CPUAccessFlags		 = D3D11_CPU_ACCESS_WRITE,
			.MiscFlags			 = 0,
			.StructureByteStride = 0,
		};

		hResult = _pDevice->CreateBuffer(
			&BUFFER_DESC,
			nullptr, // 初期データなし
			pBoneConstantBuffer_.ReleaseAndGetAddressOf()
		);

		massert(
			SUCCEEDED(hResult) && "ボーン行列用コンスタントバッファの作成に失敗 @FbxParts::InitializeConstantBuffer"
		);

		hasSkinnedMesh_ = true;
	}
}

void mtgb::FbxParts::InitializeMaterial()
{
	// マテリアルバッファの作成
	materialCount_ = pNode_->GetMaterialCount();
	pMaterial_	   = new Material[materialCount_] {};

	for (DWORD i = 0; i < materialCount_; i++)
	{
		FbxSurfaceMaterial* pSurfaceMaterial = pNode_->GetMaterial(i);

		FbxSurfacePhong* pPhong = static_cast<FbxSurfacePhong*>(pSurfaceMaterial);

		// 環境光
		FbxDouble3 ambient = FbxDouble3(0, 0, 0);
		// 拡散反射光
		FbxDouble3 diffuse = FbxDouble3(0, 0, 0);
		// 鏡面反射
		FbxDouble3 specular = FbxDouble3(0, 0, 0);

		// Ambientのプロパティを見つける
		FbxProperty prop {};

		prop = pPhong->FindProperty(FbxSurfaceMaterial::sAmbient);
		if (prop.IsValid())
		{
			ambient = pPhong->Ambient;
		}
		prop = pPhong->FindProperty(FbxSurfaceMaterial::sDiffuse);
		if (prop.IsValid())
		{
			diffuse = pPhong->Diffuse;
		}
		// MEMO: 内部でtypedef double FbxDouble されているためfloatにキャスト
		pMaterial_[i].ambient	= { (float)ambient[0], (float)ambient[1], (float)ambient[2], 1.0f };
		pMaterial_[i].diffuse	= { (float)diffuse[0], (float)diffuse[1], (float)diffuse[2], 1.0f };
		pMaterial_[i].specular	= { 0, 0, 0, 0 };
		pMaterial_[i].shininess = 0;

		if (pSurfaceMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
		{
			prop = pPhong->FindProperty(FbxSurfaceMaterial::sSpecular);
			if (prop.IsValid()) // 有効値なら
			{
				specular = pPhong->Specular;
			}

			pMaterial_[i].specular = { (float)specular[0], (float)specular[1], (float)specular[2], 1.0f };
			prop				   = pPhong->FindProperty(FbxSurfaceMaterial::sShininess);

			if (prop.IsValid())
			{
				pMaterial_[i].shininess = (float)pPhong->Shininess;
			}
			else // Shininessの値が無効なら
			{
				pMaterial_[i].shininess = 1.0f;
			}
		}
		InitializeTexture(pSurfaceMaterial, i);
	}
}

void mtgb::FbxParts::InitializeTexture(FbxSurfaceMaterial* _pMaterial, DWORD _i)
{
	// テクスチャの情報
	FbxProperty prop = _pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);

	// テクスチャ数
	int fileTextureCount { prop.GetSrcObjectCount<FbxFileTexture>() };

	if (fileTextureCount > 0) // テクスチャがあるなら
	{
		FbxFileTexture* pTexture { prop.GetSrcObject<FbxFileTexture>() };

		// ファイル名 + 拡張 だけにする
		char name[_MAX_FNAME] {};
		char ext[_MAX_EXT] {};
		_splitpath_s(pTexture->GetRelativeFileName(), nullptr, 0, nullptr, 0, name, _MAX_FNAME, ext, _MAX_EXT);
		wsprintf(name, "%s%s", name, ext);

		pMaterial_[_i].pTexture = new Texture2D {};
		pMaterial_[_i].pTexture->Load(ToWString(std::string { name }));
	}
}

void mtgb::FbxParts::InitializeSkelton()
{
	FbxDeformer* pDeformer { pMesh_->GetDeformer(0) };
	if (pDeformer == nullptr)
	{
		return; // ボーンがないため回帰
	}

	pSkin_ = static_cast<FbxSkin*>(pDeformer);

	struct POLY_INDEX
	{
		int* polyIndex;	  // ポリゴンの番号
		int* vertexIndex; // 頂点の番号
		int numRef;		  // 頂点を共有するポリゴン数
	};

	// 各ボーンの情報を取得する
	boneCount_ = pSkin_->GetClusterCount();
	ppCluster_ = new FbxCluster*[boneCount_];
	for (int i = 0; i < boneCount_; i++)
	{
		ppCluster_[i] = pSkin_->GetCluster(i);
	}

	// まず全頂点のウェイトを初期化
	for (DWORD v = 0; v < vertexCount_; v++)
	{
		for (int b = 0; b < 4; b++)
		{
			pVertexes_[v].boneIndex[b]	= 0;
			pVertexes_[v].boneWeight[b] = 0.0f;
		}
	}

	// ボーンの数に合わせてウェイト情報を準備
	for (int i = 0; i < boneCount_; i++)
	{
		// i番目のボーンが影響を与える頂点数
		int influencedVertexCount { ppCluster_[i]->GetControlPointIndicesCount() };
		// 頂点のインデックスの配列
		int* boneInfluencedVertexIndices { ppCluster_[i]->GetControlPointIndices() };
		// 頂点のウェイトの配列
		double* boneInfluencedWeights { ppCluster_[i]->GetControlPointWeights() };

		// 影響を受けている頂点の数だけループ
		for (int k = 0; k < influencedVertexCount; k++)
		{
			// k番目の頂点のインデックス
			int vertexIndex = boneInfluencedVertexIndices[k];
			// k番目の頂点のウェイト
			double weight = boneInfluencedWeights[k];

			// 1頂点最大4つまでのボーンから影響を受けるよう制限
			for (int m = 0; m < 4; m++)
			{
				if (m >= boneCount_)
					break;

				// m番目のウェイトより大きいなら上書き
				// 本来はウェイトの大きい順にソートすべき
				if (weight > pVertexes_[vertexIndex].boneWeight[m])
				{
					// 既存のデータを後ろにずらす
					for (int n = 3; n > m; n--)
					{
						pVertexes_[vertexIndex].boneIndex[n]  = pVertexes_[vertexIndex].boneIndex[n - 1];
						pVertexes_[vertexIndex].boneWeight[n] = pVertexes_[vertexIndex].boneWeight[n - 1];
					}

					if (m >= 0 && m < static_cast<int>(pVertexes_[vertexIndex].boneIndex.size()))
					{
						pVertexes_[vertexIndex].boneIndex[m]  = static_cast<uint32_t>(i);
						pVertexes_[vertexIndex].boneWeight[m] = static_cast<float>(weight);
						break;
					}
					break;
				}
			}
		}
	}

	// ウェイトの合計を1にする
	for (DWORD v = 0; v < vertexCount_; v++)
	{
		Vertex& vertex	  = pVertexes_[v];
		float totalWeight = 0.0f;
		for (int b = 0; b < 4; b++)
		{
			totalWeight += vertex.boneWeight[b];
		}

		// 合計が0でない場合、1にする
		if (totalWeight > 0.0f)
		{
			for (int b = 0; b < 4; b++)
			{
				vertex.boneWeight[b] /= totalWeight;
			}
		}

		// ウェイトの大きい順にソート
		for (int i = 0; i < 3; i++)
		{
			for (int j = i + 1; j < 4; j++)
			{
				if (vertex.boneWeight[i] < vertex.boneWeight[j])
				{
					// ボーンのウェイトとインデックスをスワップ
					std::swap(vertex.boneWeight[i], vertex.boneWeight[j]);
					std::swap(vertex.boneIndex[i], vertex.boneIndex[j]);
				}
			}
		}
	}

	// ボーン作る
	bones_.resize(boneCount_);
	for (int i = 0; i < boneCount_; i++)
	{
		FbxAMatrix matrix;
		ppCluster_[i]->GetTransformLinkMatrix(matrix);

		DirectX::XMFLOAT4X4 pose {};
		for (DWORD x = 0; x < 4; x++)
		{
			for (DWORD y = 0; y < 4; y++)
			{
				pose(x, y) = (float)matrix.Get(x, y);
			}
		}

		DirectX::XMFLOAT4X4 mirrorMat {};
		DirectX::XMStoreFloat4x4(&mirrorMat, DirectX::XMMatrixIdentity());
		mirrorMat._11 *= -1.0f;
		DirectX::XMMATRIX mirrorMatrix	 = DirectX::XMLoadFloat4x4(&mirrorMat);
		DirectX::XMMATRIX bindPoseMatrix = DirectX::XMLoadFloat4x4(&pose);

		DirectX::XMStoreFloat4x4(&pose, bindPoseMatrix);
		bones_[i].bindPose								   = DirectX::XMLoadFloat4x4(&pose);
		boneNamePair_[ppCluster_[i]->GetLink()->GetName()] = &bones_[i];
	}
}

void mtgb::FbxParts::SetBoneMatrix()
{
	using namespace DirectX;

	// 座標系変換行列を事前に準備
	XMFLOAT4X4 mirrorMatFloat {};
	XMStoreFloat4x4(&mirrorMatFloat, XMMatrixIdentity());
	mirrorMatFloat._11 *= -1.0f;
	// mirrorMatFloat._33 *= -1.0f;

	XMMATRIX mMirror = XMLoadFloat4x4(&mirrorMatFloat);

	// スケール係数の行列
	XMMATRIX scaleMatrix = XMMatrixScaling(fbxToWorldScaleFactor_, fbxToWorldScaleFactor_, fbxToWorldScaleFactor_);

	BoneMatrices boneMatrices_;
	for (int i = 0; i < boneCount_; i++)
	{
		FbxAnimEvaluator* evaluator { ppCluster_[i]->GetLink()->GetScene()->GetAnimationEvaluator() };
		FbxMatrix mCurrent { evaluator->GetNodeGlobalTransform(ppCluster_[i]->GetLink(), currentTime_) };

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
		Matrix4x4 bindPoseInv = XMMatrixInverse(nullptr, bones_[i].bindPose);

		// 最終的なボーン行列 = バインドポーズの逆行列 × 現在のポーズ
		Matrix4x4 finalBoneMatrix = bindPoseInv * currentPose;

		boneMatrices_.boneMatrices[i] = XMMatrixTranspose(finalBoneMatrix);
	}

	// コンスタントバッファに書き込み
	D3D11_MAPPED_SUBRESOURCE mappedSubResource {};
	DirectX11Draw::pContext_->Map(pBoneConstantBuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);

	memcpy_s(mappedSubResource.pData, mappedSubResource.RowPitch, (void*)&boneMatrices_, sizeof(BoneMatrices));
	DirectX11Draw::pContext_->Unmap(pBoneConstantBuffer_.Get(), 0);
}

void mtgb::FbxParts::SetAnimationTime(const FbxTime& _time)
{
	currentTime_ = _time;
}

mtgb::FbxParts::Material::~Material()
{
	delete pTexture;
}
