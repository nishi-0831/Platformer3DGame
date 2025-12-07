// RigidBody.generated.cpp
# include "RigidBody.generated.h"
# include "RigidBody.h"
# include "TypeRegistry.h"

namespace mtgb
{
	// ImGui�\��������o�^
	struct RigidBody_Register
	{
		RigidBody_Register()
		{
			RegisterShowFuncHolder::Set<RigidBody>([]( RigidBody* _target, const char* _name)
				{
								TypeRegistry::Instance().CallFunc(&_target->isNeedUpdate_, "isNeedUpdate_");
								TypeRegistry::Instance().CallFunc(&_target->velocity_, "velocity_");
								TypeRegistry::Instance().CallFunc(&_target->useGravity_, "useGravity_");
								TypeRegistry::Instance().CallFunc(&_target->isGround_, "isGround_");
								TypeRegistry::Instance().CallFunc(&_target->isKinematic_, "isKinematic_");
				});
		}
	};

	static RigidBody_Register rigidbody_register;
}
