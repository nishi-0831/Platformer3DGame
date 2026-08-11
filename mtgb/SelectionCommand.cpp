#include "SelectionCommand.h"
#include "ImGuizmoManipulator.h"
mtgb::SelectionCommand::SelectionCommand(
	std::span<const EntityId> _entityIds,
	bool _multiSelect,
	ImGuizmoManipulator& _manipulator
)
	: entityIds_ { _entityIds.begin(), _entityIds.end() }
	, multiSelect_ { _multiSelect }
	, manipulator_ { _manipulator }
{
}

void mtgb::SelectionCommand::Execute()
{
	manipulator_.Select(entityIds_, multiSelect_);
}

void mtgb::SelectionCommand::Undo()
{
	manipulator_.Deselect(entityIds_);
}

mtgb::DeselectionCommand::DeselectionCommand(std::span<const EntityId> _entityIds, ImGuizmoManipulator& _manipulator)
	: entityIds_ { _entityIds.begin(), _entityIds.end() }
	, manipulator_ { _manipulator }
{
}

void mtgb::DeselectionCommand::Execute()
{
	manipulator_.Deselect(entityIds_);
}

void mtgb::DeselectionCommand::Undo()
{
	manipulator_.Select(entityIds_, false);
}
