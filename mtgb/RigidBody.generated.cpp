// RigidBody.generated.cpp
#include "RigidBody.generated.h"
#include "RigidBody.h"
#include "TypeRegistry.h"
#include "MTImGui.h"

namespace mtgb
{
	// ImGui表示処理を登録
	struct RigidBody_Register
	{
		RigidBody_Register()
		{
			RegisterShowFuncHolder::Set<RigidBody>(
				[](RigidBody* _target, const char* _name)
				{
					PropertyDisplayRegistry::Instance().ShowProperty(&_target->isNeedUpdate_, "isNeedUpdate_");
					PropertyDisplayRegistry::Instance().ShowProperty(&_target->velocity_, "velocity_");
					PropertyDisplayRegistry::Instance().ShowProperty(&_target->useGravity_, "useGravity_");
					PropertyDisplayRegistry::Instance().ShowProperty(&_target->isGround_, "isGround_");
					PropertyDisplayRegistry::Instance().ShowProperty(&_target->isKinematic_, "isKinematic_");
				}
			);
			MTImGui::Instance().RegisterComponentViewer<RigidBody>();
		}
	};

	static RigidBody_Register rigidbody_register;
} // namespace mtgb
