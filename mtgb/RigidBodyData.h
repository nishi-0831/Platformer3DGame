#pragma once
#include "Vector3.h"
namespace mtgb
{
	struct RigidBodyData
	{
		bool isNeedUpdate;
		Vector3 velocity;
	};
}