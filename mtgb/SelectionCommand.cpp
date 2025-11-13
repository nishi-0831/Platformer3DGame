#include "SelectionCommand.h"

mtgb::SelectionCommand::SelectionCommand(const GameObjectSelectedEvent& _event, ExecuteFn _selectFunc, UndoFn _deselectFunc)
	: event_{_event}
	, selectFunc_{_selectFunc}
	, deselectFunc_{_deselectFunc}
{
}

EntityId mtgb::SelectionCommand::GetCommandTargetEntityId() const
{
	return event_.entityId;
}

mtgb::DeselectionCommand::DeselectionCommand(const GameObjectDeselectedEvent& _event, ExecuteFn _deselectFunc, UndoFn _selectFunc)
	: event_{_event}
	, deselectFunc_ {_deselectFunc}
	, selectFunc_{_selectFunc}
{
}

EntityId mtgb::DeselectionCommand::GetCommandTargetEntityId() const
{
	return event_.entityId;
}
