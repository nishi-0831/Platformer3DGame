#pragma once
#include "ColliderType.h"
#include "Vector3.h"
namespace mtgb
{
	/// <summary>
	/// Collider�̏�Ԃ݂̂�ێ�����\����
	/// </summary>
	struct ColliderData
	{
		ColliderType colliderType{ ColliderType::TYPE_AABB };// �����蔻��̌`
		bool isStatic{ false };// �ÓI�ȁATransform�s�v�ȃR���C�_�[�p�̃t���O
		ColliderTag colliderTag{ ColliderTag::GAME_OBJECT };
		Vector3 center;
		float radius;
		Vector3 extents;
	};
}