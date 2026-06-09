#pragma once
#include "ReflectionMacro.h"
#include "MTBit/BitFlag.h"

namespace mtgb
{
	enum struct [[MT_ENUM()]] GameObjectLayer : unsigned char
	{
		A,
		B,
		SCENE_VIEW, // ImGuiのゲーム画面のみに表示
	};

	using GameObjectLayerFlag = mtbit::BitFlag<GameObjectLayer>;

	GameObjectLayerFlag AllLayer();

} // namespace mtgb
