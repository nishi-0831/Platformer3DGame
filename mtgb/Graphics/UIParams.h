#pragma once
#include "Core/GameObject/GameObjectLayer.h"
namespace mtgb
{
	/// <summary>
	/// UI描画時に指定する設定
	/// </summary>
	struct UIParams
	{
		int depth					  = 0; // 小さいほど手前、大きいほど奥
		GameObjectLayerFlag layerFlag = mtbit::operator|(GameObjectLayer::A, GameObjectLayer::B); // 描画するレイヤー
	};
} // namespace mtgb