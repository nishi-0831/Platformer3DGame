#pragma once
#include "ColliderType.h"
#include "Vector3.h"
namespace mtgb
{
	/// <summary>
	/// Colliderの状態のみを保持する構造体
	/// </summary>
	struct ColliderData
	{
		ColliderType colliderType{ ColliderType::TYPE_AABB };// 当たり判定の形
		bool isStatic{ false };// 静的な、Transform不要なコライダー用のフラグ
		ColliderTag colliderTag{ ColliderTag::GAME_OBJECT };
		Vector3 center;
		float radius;
		Vector3 extents;
	};
}