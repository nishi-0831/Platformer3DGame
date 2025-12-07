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
				});
		}
	};

	static MovingFloor_Register movingfloor_register;
}
