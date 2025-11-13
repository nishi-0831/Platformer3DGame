#include "SelectionCommand.h"

mtgb::SelectionCommand::SelectionCommand(EntityId _entityId, ExecuteFn _selectFunc, UndoFn _deselectFunc)
	: entityId_{_entityId}
	, selectFunc_{_selectFunc}
	, deselectFunc_{_deselectFunc}
{
}

EntityId mtgb::SelectionCommand::GetCommandTargetEntityId() const
{
	return entityId_;
}

mtgb::DeselectionCommand::DeselectionCommand(EntityId _entityId, ExecuteFn _deselectFunc, UndoFn _selectFunc)
	: entityId_{_entityId}
	, deselectFunc_ {_deselectFunc}
	, selectFunc_{_selectFunc}
{
}

EntityId mtgb::DeselectionCommand::GetCommandTargetEntityId() const
{
	return entityId_;
}
