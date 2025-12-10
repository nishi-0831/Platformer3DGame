// MovingFloor.generated.cpp
# include "MovingFloor.generated.h"
# include "MovingFloor.h"
# include "TypeRegistry.h"
# include "MTImGui.h"

namespace mtgb
{
	// ImGuiï\é¶èàóùÇìoò^
	struct MovingFloor_Register
	{
		MovingFloor_Register()
		{
			RegisterShowFuncHolder::Set<MovingFloor>([]( MovingFloor* _target, const char* _name)
				{
				});
			MTImGui::Instance().RegisterComponentViewer<MovingFloor>();
		}
	};

	static MovingFloor_Register movingfloor_register;
}
