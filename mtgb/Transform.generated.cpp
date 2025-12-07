// Transform.generated.cpp
# include "Transform.generated.h"
# include "Transform.h"
# include "TypeRegistry.h"

namespace mtgb
{
	// ImGui�\��������o�^
	struct Transform_Register
	{
		Transform_Register()
		{
			RegisterShowFuncHolder::Set<Transform>([]( Transform* _target, const char* _name)
				{
								TypeRegistry::Instance().CallFunc(&_target->parent, "parent");
								TypeRegistry::Instance().CallFunc(&_target->position, "position");
								TypeRegistry::Instance().CallFunc(&_target->scale, "scale");
								TypeRegistry::Instance().CallFunc(&_target->rotate, "rotate");
				});
		}
	};

	static Transform_Register transform_register;
}
