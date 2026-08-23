#include "MeshAsset.h"
#include "MTStringUtility.h"
#include "MTAssert.h"
#include "ReleaseUtility.h"
mtgb::MeshAsset::~MeshAsset() {}

void LoadSkelton(mtgb::MeshAsset* _asset)
{
	FbxDeformer* pDeformer { _asset->pFbxMesh->GetDeformer(0) };
	if (pDeformer == nullptr)
	{
		return; // ボーンがないため回帰
	}

	FbxSkin* pSkin_	 = static_cast<FbxSkin*>(pDeformer);
	_asset->pFbxSkin = pSkin_;

	// 各ボーンの情報を取得する
	_asset->boneCount = pSkin_->GetClusterCount();
	_asset->ppCluster = new FbxCluster*[_asset->boneCount];
	for (size_t i = 0; i < _asset->boneCount; i++)
	{
		_asset->ppCluster[i] = pSkin_->GetCluster(i);
	}

	// まず全頂点のウェイトを初期化
	for (size_t v = 0; v < _asset->vertexCount; v++)
	{
		for (int b = 0; b < 4; b++)
		{
			_asset->vertices[v].boneIndex[b]  = 0;
			_asset->vertices[v].boneWeight[b] = 0.0f;
		}
	}

	// ボーンの数に合わせてウェイト情報を準備
	for (size_t i = 0; i < _asset->boneCount; i++)
	{
		// i番目のボーンが影響を与える頂点数
		int influencedVertexCount { _asset->ppCluster[i]->GetControlPointIndicesCount() };
		// 頂点のインデックスの配列
		int* boneInfluencedVertexIndices { _asset->ppCluster[i]->GetControlPointIndices() };
		// 頂点のウェイトの配列
		double* boneInfluencedWeights { _asset->ppCluster[i]->GetControlPointWeights() };

		// 影響を受けている頂点の数だけループ
		for (size_t k = 0; k < influencedVertexCount; k++)
		{
			// k番目の頂点のインデックス
			int vertexIndex = boneInfluencedVertexIndices[k];
			// k番目の頂点のウェイト
			double weight = boneInfluencedWeights[k];

			// 1頂点最大4つまでのボーンから影響を受けるよう制限
			for (int m = 0; m < 4; m++)
			{
				if (m >= _asset->boneCount)
					break;

				// m番目のウェイトより大きいなら上書き
				// 本来はウェイトの大きい順にソートすべき
				if (weight > _asset->vertices[vertexIndex].boneWeight[m])
				{
					// 既存のデータを後ろにずらす
					for (int n = 3; n > m; n--)
					{
						_asset->vertices[vertexIndex].boneIndex[n]	= _asset->vertices[vertexIndex].boneIndex[n - 1];
						_asset->vertices[vertexIndex].boneWeight[n] = _asset->vertices[vertexIndex].boneWeight[n - 1];
					}

					if (m >= 0 && m < static_cast<int>(_asset->vertices[vertexIndex].boneIndex.size()))
					{
						_asset->vertices[vertexIndex].boneIndex[m]	= static_cast<uint32_t>(i);
						_asset->vertices[vertexIndex].boneWeight[m] = static_cast<float>(weight);
						break;
					}
					break;
				}
			}
		}
	}

	// ウェイトの合計を1にする
	for (size_t v = 0; v < _asset->vertexCount; v++)
	{
		mtgb::Vertex& vertex = _asset->vertices[v];
		float totalWeight	 = 0.0f;
		for (size_t b = 0; b < 4; b++)
		{
			totalWeight += vertex.boneWeight[b];
		}

		// 合計が0でない場合、1にする
		if (totalWeight > 0.0f)
		{
			for (size_t b = 0; b < 4; b++)
			{
				vertex.boneWeight[b] /= totalWeight;
			}
		}

		// ウェイトの大きい順にソート
		for (size_t i = 0; i < 3; i++)
		{
			for (size_t j = i + 1; j < 4; j++)
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
	_asset->bones.resize(_asset->boneCount);
	for (size_t i = 0; i < _asset->boneCount; i++)
	{
		FbxAMatrix matrix;
		_asset->ppCluster[i]->GetTransformLinkMatrix(matrix);

		DirectX::XMFLOAT4X4 pose {};
		for (size_t x = 0; x < 4; x++)
		{
			for (size_t y = 0; y < 4; y++)
			{
				pose(x, y) = static_cast<float>(matrix.Get(x, y));
			}
		}

		DirectX::XMFLOAT4X4 mirrorMat {};
		DirectX::XMStoreFloat4x4(&mirrorMat, DirectX::XMMatrixIdentity());
		mirrorMat._11 *= -1.0f;
		DirectX::XMMATRIX mirrorMatrix	 = DirectX::XMLoadFloat4x4(&mirrorMat);
		DirectX::XMMATRIX bindPoseMatrix = DirectX::XMLoadFloat4x4(&pose);

		DirectX::XMStoreFloat4x4(&pose, bindPoseMatrix);
		_asset->bones[i].bindPose										 = DirectX::XMLoadFloat4x4(&pose);
		_asset->boneNamePair[_asset->ppCluster[i]->GetLink()->GetName()] = &_asset->bones[i];
	}
}

void LoadVertex(mtgb::MeshAsset* _asset)
{
	_asset->vertexCount = _asset->polygonCount * 3;
	_asset->vertices.resize(_asset->vertexCount);
	FbxDeformer* pDeformer = _asset->pFbxMesh->GetDeformer(0);
	_asset->hasSkinnedMesh = (pDeformer != nullptr);

	for (uint32_t poly = 0; poly < _asset->polygonCount; poly++)
	{
		for (uint32_t vertex = 0; vertex < 3; vertex++)
		{
			int index { _asset->pFbxMesh->GetPolygonVertex(poly, vertex) };

			// インデックスが範囲内かチェック
			if (index < 0 || index >= static_cast<int>(_asset->vertexCount))
			{
				massert(false && "Vertex index out of range in FbxParts::InitializeVertexBuffer");
				continue;
			}

			// 頂点の座標
			FbxVector4 position				 = _asset->pFbxMesh->GetControlPointAt(index);
			_asset->vertices[index].position = {
				static_cast<float>(position[0]) * _asset->fbxToWorldScaleFactor,
				static_cast<float>(position[1]) * _asset->fbxToWorldScaleFactor,
				static_cast<float>(position[2]) * _asset->fbxToWorldScaleFactor,
				1.0f,
			};

			// 頂点の法線
			FbxVector4 normal;
			_asset->pFbxMesh->GetPolygonVertexNormal(poly, vertex, normal);
			_asset->vertices[index].normal = { static_cast<float>(normal[0]),
											   static_cast<float>(normal[1]),
											   static_cast<float>(normal[2]),
											   0.0f };

			// スキニング関連の初期化
			if (_asset->hasSkinnedMesh)
			{
				for (int i = 0; i < 4; ++i)
				{
					_asset->vertices[index].boneIndex[i]  = 0;
					_asset->vertices[index].boneWeight[i] = 0.0f;
				}
			}
			else
			{
				// スキニング無しの場合はデフォルト値
				_asset->vertices[index].boneIndex[0]  = 0;
				_asset->vertices[index].boneWeight[0] = 1.0f;
				for (int i = 1; i < 4; ++i)
				{
					_asset->vertices[index].boneIndex[i]  = 0;
					_asset->vertices[index].boneWeight[i] = 0.0f;
				}
			}
		}
	}

	// UV座標の処理

	int uvCount { _asset->pFbxMesh->GetTextureUVCount() };
	FbxLayerElementUV* pUV { _asset->pFbxMesh->GetLayer(0)->GetUVs() };
	if (pUV == nullptr || uvCount == 0)
	{
		// UVが存在しない場合、デフォルト値を設定
		for (uint32_t i = 0; i < _asset->vertexCount; i++)
		{
			_asset->vertices[i].uv = { 0.0f, 0.0f };
		}
	}
	else
	{
		FbxLayerElement::EMappingMode mappingMode	  = pUV->GetMappingMode();
		FbxLayerElement::EReferenceMode referenceMode = pUV->GetReferenceMode();
		bool useIndex								  = referenceMode != FbxLayerElement::eDirect;
		int polyCount								  = _asset->pFbxMesh->GetPolygonCount();
		int uvNum									  = pUV->GetDirectArray().GetCount();
		_asset->indexCount							  = useIndex ? pUV->GetIndexArray().GetCount() : 0;

		if (mappingMode == FbxLayerElement::eByControlPoint)
		{
			for (int polyIndex = 0; polyIndex < polyCount; polyIndex++)
			{
				int polySize = _asset->pFbxMesh->GetPolygonSize(polyIndex);
				for (int vertIdx = 0; vertIdx < polySize; vertIdx++)
				{
					// 頂点のインデックスを取得
					int polyVertIdx = _asset->pFbxMesh->GetPolygonVertex(polyIndex, vertIdx);
					// UVのインデックスを取得。eDirectの場合は頂点のインデックスをそのまま使う。そうでない場合はインデックスバッファから取得
					int UVIdx = useIndex ? pUV->GetIndexArray().GetAt(polyVertIdx) : polyVertIdx;

					FbxVector2 UVValue = pUV->GetDirectArray().GetAt(UVIdx);

					_asset->vertices[polyVertIdx].uv = { static_cast<float>(UVValue[0]),
														 static_cast<float>(1.0 - UVValue[1]) };
				}
			}
		}
		else if (mappingMode == FbxLayerElement::eByPolygonVertex)
		{
			int polyIdxCounter = 0;
			for (int polyIdx = 0; polyIdx < polyCount; polyIdx++)
			{
				int polySize = _asset->pFbxMesh->GetPolygonSize(polyIdx);
				for (int vertIdx = 0; vertIdx < polySize; vertIdx++)
				{
					if (polyIdxCounter < _asset->indexCount)
					{
						int polyVertIdx = _asset->pFbxMesh->GetPolygonVertex(polyIdx, vertIdx);
						int UVIdx		= useIndex ? pUV->GetIndexArray().GetAt(polyIdxCounter) : polyIdxCounter;

						FbxVector2 UVValue				 = pUV->GetDirectArray().GetAt(UVIdx);
						_asset->vertices[polyVertIdx].uv = { static_cast<float>(UVValue[0]),
															 static_cast<float>(1.0 - UVValue[1]) };
						polyIdxCounter++;
					}
				}
			}
		}
		// 骨の処理
		if (_asset->hasSkinnedMesh)
		{
			LoadSkelton(_asset);
		}
	}
}

void LoadTexture(mtgb::MeshAsset* _asset, FbxSurfaceMaterial* _pMaterial, DWORD _i)
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

		_asset->materials[_i].pTexture = new mtgb::Texture2D {};
		_asset->materials[_i].pTexture->Load(mtgb::ToWString(std::string { name }));
	}
}
void LoadMaterial(mtgb::MeshAsset* _asset)
{
	// マテリアルバッファの作成
	_asset->materialCount = _asset->pFbxNode->GetMaterialCount();
	_asset->materials.resize(_asset->materialCount);

	for (DWORD i = 0; i < _asset->materialCount; i++)
	{
		FbxSurfaceMaterial* pSurfaceMaterial = _asset->pFbxNode->GetMaterial(i);

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
		_asset->materials[i].ambient   = { (float)ambient[0], (float)ambient[1], (float)ambient[2], 1.0f };
		_asset->materials[i].diffuse   = { (float)diffuse[0], (float)diffuse[1], (float)diffuse[2], 1.0f };
		_asset->materials[i].specular  = { 0, 0, 0, 0 };
		_asset->materials[i].shininess = 0;

		if (pSurfaceMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
		{
			prop = pPhong->FindProperty(FbxSurfaceMaterial::sSpecular);
			if (prop.IsValid()) // 有効値なら
			{
				specular = pPhong->Specular;
			}

			_asset->materials[i].specular = { (float)specular[0], (float)specular[1], (float)specular[2], 1.0f };
			prop						  = pPhong->FindProperty(FbxSurfaceMaterial::sShininess);

			if (prop.IsValid())
			{
				_asset->materials[i].shininess = (float)pPhong->Shininess;
			}
			else // Shininessの値が無効なら
			{
				_asset->materials[i].shininess = 1.0f;
			}
		}
		LoadTexture(_asset, pSurfaceMaterial, i);
	}
}

void LoadIndex(mtgb::MeshAsset* _asset)
{
	_asset->ppIndexBuffer.resize(_asset->materialCount);
	_asset->ppIndexData.resize(_asset->materialCount);

	int count { 0 };
	for (size_t i = 0; i < _asset->materialCount; i++)
	{
		count = 0;
		uint32_t* pIndex { new uint32_t[_asset->polygonCount * 3] {} };
		for (size_t j = 0; j < _asset->polygonCount; j++)
		{
			FbxLayerElementMaterial* material { _asset->pFbxMesh->GetLayer(0)->GetMaterials() };
			int materialId { material->GetIndexArray().GetAt(j) };
			if (materialId == i)
			{
				for (size_t k = 0; k < 3; k++)
				{
					pIndex[count + k] = _asset->pFbxMesh->GetPolygonVertex(j, k);
				}
				count += 3;
			}
		}
		_asset->materials[i].polygonCount = count / 3;
		_asset->ppIndexData[i].resize(count);
		memcpy(_asset->ppIndexData[i].data(), pIndex, sizeof(uint32_t) * count);
		SAFE_DELETE_ARRAY(pIndex);
	}
}

mtgb::MeshAsset* mtgb::MeshAsset::LoadFromFbx(FbxNode* _pNode, double _unitScaleFactor)
{
	if (_pNode == nullptr)
	{
		return nullptr;
	}

	FbxMesh* mesh = _pNode->GetMesh();
	if (mesh == nullptr)
	{
		return nullptr;
	}

	MeshAsset* asset			 = new MeshAsset();
	asset->unitScaleFactor		 = _unitScaleFactor;
	asset->pFbxMesh				 = mesh;
	asset->pFbxNode				 = _pNode;
	float fbxToWorld			 = static_cast<float>(1.0 / _unitScaleFactor);
	asset->fbxToWorldScaleFactor = fbxToWorld;
	asset->vertexCount			 = mesh->GetControlPointsCount();
	asset->polygonCount			 = mesh->GetPolygonCount();
	asset->materialCount		 = _pNode->GetMaterialCount();
	LoadMaterial(asset);
	LoadVertex(asset);
	LoadIndex(asset);

	return asset;
}

bool mtgb::MeshAsset::TryGetBonePosition(std::string_view _boneName, Vector3* _pPosition)
{
	for (int i = 0; i < boneCount; i++)
	{
		// 見つかった！
		if (_boneName == ppCluster[i]->GetLink()->GetName())
		{
			FbxAMatrix m {};
			ppCluster[i]->GetTransformLinkMatrix(m);

			_pPosition->x = static_cast<float>(m[3][0]);
			_pPosition->y = static_cast<float>(m[3][1]);
			_pPosition->z = static_cast<float>(m[3][2]);

			return true;
		}
	}

	return false;
}

bool mtgb::MeshAsset::TryGetBonePositionAtNow(std::string_view _boneName, Vector3* _pPosition)
{
	auto itr { boneNamePair.find(_boneName) };
	if (itr != boneNamePair.end()) // end じゃないなら見つかった
	{
		Matrix4x4 m {};
		_pPosition->x = DirectX::XMVectorGetX(m.r[3]);
		_pPosition->y = DirectX::XMVectorGetY(m.r[3]);
		_pPosition->z = DirectX::XMVectorGetZ(m.r[3]);

		return true;
	}
	return false;
}

void mtgb::MeshAsset::CreateGpuResources(ID3D11Device* pDevice)
{
	// 頂点バッファ
	{
		D3D11_BUFFER_DESC bd {};
		bd.Usage		  = D3D11_USAGE_DEFAULT;
		bd.ByteWidth	  = static_cast<UINT>(sizeof(Vertex) * vertices.size());
		bd.BindFlags	  = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA initData {};
		initData.pSysMem = vertices.data();

		ID3D11Buffer* pBuf = nullptr;
		HRESULT hr		   = pDevice->CreateBuffer(&bd, &initData, &pBuf);
		assert(SUCCEEDED(hr));
		if (SUCCEEDED(hr))
		{
			pVertexBuffer.Attach(pBuf);
		}
	}

	// インデックスバッファ
	for (size_t i = 0; i < materialCount; i++)
	{
		D3D11_BUFFER_DESC bd {};
		bd.Usage		  = D3D11_USAGE_DEFAULT;
		bd.ByteWidth	  = static_cast<UINT>(sizeof(uint32_t) * ppIndexData[i].size());
		bd.BindFlags	  = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA initData {};
		initData.pSysMem = ppIndexData[i].data();

		ID3D11Buffer* pBuf = nullptr;
		HRESULT hr		   = pDevice->CreateBuffer(&bd, &initData, &pBuf);
		assert(SUCCEEDED(hr));
		if (SUCCEEDED(hr))
		{
			ppIndexBuffer[i].Attach(pBuf);
		}
	}
}

void mtgb::MeshAsset::ReleaseGpuResources()
{
	pVertexBuffer.Reset();
	for (auto buf : ppIndexBuffer)
	{
		buf.Reset();
	}
}
mtgb::Material::~Material()
{
	delete pTexture;
}
