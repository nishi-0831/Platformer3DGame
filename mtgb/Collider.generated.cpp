// Collider.generated.cpp
# include "Collider.generated.h"
# include "Collider.h"
# include "TypeRegistry.h"
# include "MTImGui.h"

namespace mtgb
{
	// ImGui•\¦ˆ—‚ğ“o˜^
	struct Collider_Register
	{
		Collider_Register()
		{
			RegisterShowFuncHolder::Set<Collider>([]( Collider* _target, const char* _name)
				{
								TypeRegistry::Instance().CallFunc(&_target->colliderType_, "colliderType_");
								TypeRegistry::Instance().CallFunc(&_target->isStatic_, "isStatic_");
								TypeRegistry::Instance().CallFunc(&_target->colliderTag_, "colliderTag_");
								TypeRegistry::Instance().CallFunc(&_target->center_, "center_");
								TypeRegistry::Instance().CallFunc(&_target->radius_, "radius_");
								TypeRegistry::Instance().CallFunc(&_target->extents_, "extents_");
				});
			MTImGui::Instance().RegisterComponentViewer<Collider>();
		}
	};

	static Collider_Register collider_register;
}
