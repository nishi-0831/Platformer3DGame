#pragma once
#include "MTBit/BitFlag.h"

namespace mtgb
{
	enum struct GameObjectLayer : unsigned char
	{
		A,
		B,
		SceneView, // ImGui�̃Q�[����ʂ݂̂ɕ\��
	};

	using GameObjectLayerFlag = mtbit::BitFlag<GameObjectLayer>;

	GameObjectLayerFlag AllLayer();
	
}

