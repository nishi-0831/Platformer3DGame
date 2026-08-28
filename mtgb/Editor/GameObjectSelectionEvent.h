#pragma once

#include "Core/Entity.h"
#include "Editor/SelectionMode.h"
#include <vector>
namespace mtgb
{
	struct GameObjectSelectedEvent
	{
		std::vector<EntityId> entityIds;
		SelectionMode selectionMode;
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