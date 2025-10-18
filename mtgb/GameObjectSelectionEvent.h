#pragma once

#include "Entity.h"

namespace mtgb
{
	struct GameObjectSelectedEvent
	{
		EntityId entityId{};
	};

	struct GameObjectDeselectedEvent
	{
		EntityId entityId{};
	};

	struct SelectionClearedEvent{};
}