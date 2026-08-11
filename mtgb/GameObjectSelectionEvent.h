#pragma once

#include "Entity.h"
#include <vector>
namespace mtgb
{
	struct GameObjectSelectedEvent
	{
		std::vector<EntityId> entityIds;
		bool multiSelect;
	};

	struct GameObjectDeselectedEvent
	{
		std::vector<EntityId> entityIds;
	};

	struct SelectionClearedEvent
	{
	};

	struct GameObjectCreatedEvent
	{
		EntityId entityId;
	};
	struct GameObjectDeletedEvent
	{
		EntityId entityId;
	};
} // namespace mtgb