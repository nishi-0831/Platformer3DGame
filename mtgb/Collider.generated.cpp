// Collider.generated.cpp
#include "Collider.generated.h"
#include "Collider.h"
#include "TypeRegistry.h"
#include "MTImGui.h"

namespace mtgb
{
	// ImGui表示処理を登録
	struct Collider_Register
	{
		Collider_Register()
		{
			RegisterShowFuncHolder::Set<Collider>(
				[](Collider* _target, const char* _name)
				{
					PropertyDisplayRegistry::Instance().ShowProperty(&_target->colliderType_, "colliderType_");
					PropertyDisplayRegistry::Instance().ShowProperty(&_target->isStatic_, "isStatic_");
					PropertyDisplayRegistry::Instance().ShowProperty(&_target->colliderTag_, "colliderTag_");
					PropertyDisplayRegistry::Instance().ShowProperty(&_target->center_, "center_");
					PropertyDisplayRegistry::Instance().ShowProperty(&_target->radius_, "radius_");
					PropertyDisplayRegistry::Instance().ShowProperty(&_target->extents_, "extents_");
				}
			);
			MTImGui::Instance().RegisterComponentViewer<Collider>();
		}
	};

	static Collider_Register collider_register;
} // namespace mtgb
