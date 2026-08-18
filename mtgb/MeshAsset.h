#pragma once
#include <array>
#include <wrl/client.h>
#include <vector>
#include <fbxsdk.h>

#include <d3d11.h>
#include <unordered_map>
#include "Vector4.h"
#include "StringComparators.h"

#include "Vector3.h"
#include "Vector2.h"
#include "Matrix4x4.h"
#include "Texture2D.h"
#include <memory>
#include <string>
#include <cstdint>

using Microsoft::WRL::ComPtr;

struct ID3D11Buffer;
namespace mtgb
{
	inline constexpr UINT MAX_BONE_COUNT = 128;

	struct BoneMatrices
	{
		Matrix4x4 boneMatrices[MAX_BONE_COUNT]; // 最大ボーン数
	};

	struct Vertex
	{
		Vector4 position; // 座標
		Vector4 normal;	  // 法線
		Vector2 uv;		  // uv座標
		std::array<uint32_t, 4> boneIndex;
		std::array<float, 4> boneWeight;
	};
	struct Material
	{
		~Material();
		uint32_t polygonCount; // ポリゴン数
		Vector4 diffuse;	   // 拡散反射光への反射強度
		Vector4 ambient;	   // 環境光への反射強度
		Vector4 specular;	   // 鏡面反射光
		float shininess;	   // ハイライトの強さ
		Texture2D* pTexture;
	};
	/// <summary>
	/// ボーン (関節そのもの)
	/// </summary>
	struct Bone
	{
		// REF: https://help.autodesk.com/view/MAYACRE/JPN/?guid=GUID-36808BCC-ACF9-4A9E-B0D8-B8F509FEC0D5
		Matrix4x4 bindPose; // 初期ポーズ時のボーン変換行列
	};
	/// <summary>
	/// FBX から抽出したメッシュ資産。CPU 側データと GPU リソースを保持する。
	/// </summary>
	struct MeshAsset
	{
		std::vector<Vertex> vertices;
		std::vector<Material> materials;
		std::vector<ComPtr<ID3D11Buffer>> ppIndexBuffer;
		std::vector<std::vector<uint32_t>> ppIndexData; // インデックス情報

		std::unordered_map<std::string, int, TransparentStringHash, TransparentStringEq> boneNameToIndex;

		FbxMesh* pFbxMesh = nullptr;
		FbxNode* pFbxNode = nullptr;
		FbxSkin* pFbxSkin = nullptr;
		FbxCluster** ppCluster; // クラスタ情報 (関節ごとに関連つけられた頂点情報)
		int boneCount;			// FBX に含まれている関節の数

		// GPU resources
		ComPtr<ID3D11Buffer> pVertexBuffer;
		std::vector<Bone> bones; // 各関節の情報配列

		float fbxToWorldScaleFactor = 1.0f;
		double unitScaleFactor		= 1.0;
		bool hasSkinnedMesh;
		uint32_t vertexCount;	// 頂点数
		uint32_t polygonCount;	// ポリゴン数
		uint32_t indexCount;	// インデックス数
		uint32_t materialCount; // マテリアル数
		~MeshAsset();
		std::unordered_map<std::string, Bone*, TransparentStringHash, TransparentStringEq> boneNamePair;

		static MeshAsset* LoadFromFbx(FbxNode* pNode, double unitScaleFactor);
		/// <summary>
		/// ボーンのアニメーション無しのときの座標を取得する
		/// </summary>
		/// <param name="_boneName">名前</param>
		/// <param name="_pPosition">座標の参照渡し</param>
		/// <returns>ボーンの取得に成功した true / false</returns>
		bool TryGetBonePosition(std::string_view _boneName, Vector3* _pPosition);
		/// <summary>
		/// ボーンのアニメーション中の座標を取得する
		/// </summary>
		/// <param name="_boneName">名前</param>
		/// <param name="_pPosition">座標の参照渡し</param>
		/// <returns>ボーンの取得に成功した true / false</returns>
		bool TryGetBonePositionAtNow(std::string_view _boneName, Vector3* _pPosition);
		// GPU リソースを作成 / 解放
		void CreateGpuResources(ID3D11Device* pDevice);
		void ReleaseGpuResources();
	};

} // namespace mtgb