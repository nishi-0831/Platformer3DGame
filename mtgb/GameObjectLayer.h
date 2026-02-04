#pragma once
#include "MTBit/BitFlag.h"

namespace mtgb
{
	enum struct GameObjectLayer : unsigned char
	{
		A,
		B,
		SCENE_VIEW, // ImGuiのゲーム画面のみに表示
	};

	using GameObjectLayerFlag = mtbit::BitFlag<GameObjectLayer>;

	GameObjectLayerFlag AllLayer();

} // namespace mtgb
