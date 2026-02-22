// Transform.generated.cpp
// # include "Transform.generated.h"
#include "Transform.h"
#include "TypeRegistry.h"
#include "MTImGui.h"

namespace mtgb
{
	// ImGui表示処理を登録
	struct Transform_Register
	{
		Transform_Register()
		{
			RegisterShowFuncHolder::Set<Transform>(
				[](Transform* _target, const char* _name)
				{
					PropertyDisplayRegistry::Instance().ShowProperty(&_target->parent, "parent");
					PropertyDisplayRegistry::Instance().ShowProperty(&_target->position, "position");
					PropertyDisplayRegistry::Instance().ShowProperty(&_target->scale, "scale");
					PropertyDisplayRegistry::Instance().ShowProperty(&_target->rotate, "rotate");
				}
			);
			MTImGui::Instance().RegisterComponentViewer<Transform>();
		}
	};

	static Transform_Register transform_register;
} // namespace mtgb
