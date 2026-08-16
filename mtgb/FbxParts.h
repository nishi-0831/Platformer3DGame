#pragma once
#include "cmtgb.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "IShader.h"
#include "Texture2D.h"
#include "StringComparators.h"
#include <unordered_map>
#include <fbxsdk.h>
#include <wrl/client.h>
#include <array>
#include "Vector2.h"
#include "MeshAsset.h"
using Microsoft::WRL::ComPtr;

namespace fbxsdk
{
	class FbxCluster;
}

namespace mtgb
{
	class Fbx;
	class Transform;
	class DirectX11Draw;

	class FbxParts
	{
		friend DirectX11Draw;

	  public:
	  public:
		FbxParts(FbxNode* _parent, double _unitScaleFactor);
		~FbxParts();

		FbxNode* GetNode() const
		{
			return pNode_;
		}

		/// <summary>
		/// 試しにボーンのアニメーション無しのときの座標を取得する
		/// </summary>
		/// <param name="_boneName">名前</param>
		/// <param name="_pPosition">座標の参照渡し</param>
		/// <returns>ボーンの取得に成功した true / false</returns>
		bool TryGetBonePosition(std::string_view _boneName, Vector3* _pPosition);
		/// <summary>
		/// 試しにボーンのアニメーション中の座標を取得する
		/// </summary>
		/// <param name="_boneName">名前</param>
		/// <param name="_pPosition">座標の参照渡し</param>
		/// <returns>ボーンの取得に成功した true / false</returns>
		bool TryGetBonePositionAtNow(std::string_view _boneName, Vector3* _pPosition);

		/// <summary>
		/// Fbxのスキンを取得
		/// </summary>
		/// <returns>スキンのポインタ</returns>
		FbxSkin* GetSkin()
		{
			return pMeshAsset_->pFbxSkin;
		}

	  private:
		void SetBoneMatrix();
		bool HasSkinnedMesh() const
		{
			return pMeshAsset_->hasSkinnedMesh;
		}
		double unitScaleFactor_;
		FbxNode* pNode_;

	  private:
		void SetAnimationTime(const FbxTime& _time);
		MeshAsset* pMeshAsset_;

		FbxTime currentTime_; // 現在設定されているアニメーションの時間
	};

} // namespace mtgb
