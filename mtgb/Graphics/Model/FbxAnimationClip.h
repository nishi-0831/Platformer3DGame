#pragma once
#include <string>

namespace fbxsdk
{
	class FbxAnimStack;
}
namespace mtgb
{
	struct FbxAnimationClip
	{
		FbxAnimationClip(FbxAnimStack* _pAnimStack);
		// アニメーション名
		std::string name;
		// 開始フレーム
		int startFrame;
		// 終了フレーム
		int endFrame;
		// アニメーションクリップのデータがまとまったクラス
		FbxAnimStack* pAnimStack;
	};
} // namespace mtgb