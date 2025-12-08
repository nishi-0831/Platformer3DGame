// MovingFloor.generated.cpp
# include "MovingFloor.generated.h"
# include "MovingFloor.h"
# include "TypeRegistry.h"

namespace mtgb
{
	// ImGui�\��������o�^
	struct MovingFloor_Register
	{
		MovingFloor_Register()
		{
			RegisterShowFuncHolder::Set<MovingFloor>([]( MovingFloor* _target, const char* _name)
				{
								TypeRegistry::Instance().CallFunc(&_target->duration_, "duration_");
								TypeRegistry::Instance().CallFunc(&_target->to_, "to_");
								TypeRegistry::Instance().CallFunc(&_target->from_, "from_");
				});
		}
	};

	static MovingFloor_Register movingfloor_register;
}
