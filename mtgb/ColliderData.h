#pragma once
#include "Collider.h"
#include "Vector3.h"
namespace mtgb
{
	/// <summary>
	/// Collider�̏�Ԃ݂̂�ێ�����\����
	/// </summary>
	struct ColliderData
	{
		ColliderType type{ ColliderType::TYPE_AABB };
		bool isStatic{ false };
		ColliderTag colliderTag{ ColliderTag::GAME_OBJECT };
		Vector3 center;
		float radius;
		float extents;
	};
}