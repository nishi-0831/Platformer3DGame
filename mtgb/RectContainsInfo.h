#pragma once
#include "Vector3.h"
#include "Vector2F.h"
#include "Entity.h"
namespace mtgb
{
	/// <summary>
	/// �X�N���[�����W�n�̊܂܂�Ă���Ώۂ̏��
	/// </summary>
	struct ScreenCoordContainsInfo
	{
		Vector3 worldPos{};//���[���h���W�ł̍��W
		Vector3 screenPos{};//�X�N���[�����W�n�ł̍��W
		EntityId entityId;
		ScreenCoordContainsInfo() = default;
		ScreenCoordContainsInfo(Vector3 _worldPos, Vector3 _screenPos,EntityId _entityId)
			:worldPos{ _worldPos }
			,screenPos{ _screenPos}
			,entityId{_entityId}
		{ }
	};
}