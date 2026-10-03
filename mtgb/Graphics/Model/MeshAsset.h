#pragma once
#include <array>
#include <wrl/client.h>
#include <vector>
#include <fbxsdk.h>

#include <d3d11.h>
#include <unordered_map>
#include "Math/Vector4.h"
#include "Utility/StringComparators.h"

#include "Math/Vector3.h"
#include "Math/Vector2.h"
#include "Math/Matrix4x4.h"
#include "Graphics/Image/Texture2D.h"
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
		Vector4 position;				   // 座標
		Vector4 normal;					   // 法線
		Vector2 uv;						   // uv座標
		std::array<uint32_t, 4> boneIndex; // 頂点に関連付けられたボーンのインデックス
		std::array<float, 4> boneWeight;   // 頂点に関連付けられたボーンの重み
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
	/// ボーン(関節そのもの)
	/// </summary>
	struct Bone
	{
		// REF: https://help.autodesk.com/view/MAYACRE/JPN/?guid=GUID-36808BCC-ACF9-4A9E-B0D8-B8F509FEC0D5
		Matrix4x4 bindPose; // 初期ポーズ時のボーン変換行列
	};
	/// <summary>
	/// 3Dモデルのデータをまとめた構造体
	/// </summary>
	struct MeshAsset
	{
		~MeshAsset();

		/// <summary>
		/// Fbxからインスタンス作成
		/// </summary>
		/// <param name="pNode">Fbxファイルから作成したFbxNode</param>
		/// <param name="unitScaleFactor">ゲームのスケールを1としたときの、Fbxモデルのスケール係数</param>
		/// <returns></returns>
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
		/// <summary>
		/// GPUに渡す用のリソースを作成
		/// </summary>
		/// <param name="pDevice"></param>
		void CreateGpuResources(ID3D11Device* pDevice);
		/// <summary>
		/// GPUに渡す用のリソースを解放
		/// </summary>
		void ReleaseGpuResources();

		// GPUに渡すリソース
		std::vector<ComPtr<ID3D11Buffer>> ppIndexBuffer;
		ComPtr<ID3D11Buffer> pVertexBuffer;

		bool hasSkinnedMesh;   // スキニングするメッシュか否か
		uint32_t vertexCount;  // 頂点数
		uint32_t polygonCount; // ポリゴン数
		std::vector<Vertex> vertices;
		// マテリアルの数
		uint32_t materialCount;
		// マテリアルの配列
		std::vector<Material> materials;
		// インデックス数
		uint32_t indexCount;
		std::vector<std::vector<uint32_t>> ppIndexData;

		// ボーンの情報
		std::vector<Bone> bones; // ボーンの配列
		int boneCount;			 // ボーンの数
		// ボーンの名前->ボーン
		std::unordered_map<std::string, Bone*, TransparentStringHash, TransparentStringEq> boneNamePair;

		// FBX関連のデータ
		FbxMesh* pFbxMesh = nullptr;
		FbxNode* pFbxNode = nullptr;
		FbxSkin* pFbxSkin = nullptr;
		FbxCluster** ppCluster; // クラスタ(ボーンごとに関連つけられた頂点情報)
		// Fbxのスケールからゲーム内のスケールに変換するための係数
		float fbxToWorldScaleFactor = 1.0f;
	};

} // namespace mtgb