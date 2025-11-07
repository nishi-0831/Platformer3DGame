#pragma once
#include "Vector3.h"
namespace mtgb
{
	struct RigidBodyData
	{
		bool isNeedUpdate;
		Vector3 velocity;
		bool useGravity;
		bool isGround;
		bool isKinematic{ true };
	};
}