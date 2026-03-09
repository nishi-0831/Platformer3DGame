#include "SelectionCommand.h"


mtgb::EntityId mtgb::SelectionCommand::GetCommandTargetEntityId() const
{
	return entityId_;
}

mtgb::EntityId mtgb::DeselectionCommand::GetCommandTargetEntityId() const
{
	return entityId_;
}
