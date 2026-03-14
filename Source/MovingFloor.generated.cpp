// MovingFloor.generated.cpp
#include "MovingFloor.generated.h"
#include "MovingFloor.h"
#include "PropertyDisplayRegistry.h"
#include "MTImGui.h"

namespace mtgb
{
	// ImGui表示処理を登録
	struct MovingFloor_Register
	{
		MovingFloor_Register()
		{
			RegisterShowFuncHolder::Set<MovingFloor>([](MovingFloor* _target, const char* _name) {});
			MTImGui::RegisterComponentViewer<MovingFloor>();
		}
	};

	static MovingFloor_Register movingfloor_register;
} // namespace mtgb
