#pragma once
#include "Vector3.h"
#include "Quaternion.h"
#include "Entity.h"

namespace mtgb
{
	/// <summary>
	/// Transform�̏�Ԃ݂̂�ێ�����\����
	/// </summary>
	struct TransformData
	{
        EntityId parent{ INVALID_ENTITY };
        Vector3 position{ 0.0f, 0.0f, 0.0f };
        Vector3 scale{ 1.0f, 1.0f, 1.0f };
        Quaternion rotate{};
	};


}
