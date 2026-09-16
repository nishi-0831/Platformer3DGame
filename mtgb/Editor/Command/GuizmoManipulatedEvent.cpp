#include "GuizmoManipulatedEvent.h"
#include "Utility/ReleaseUtility.h"

mtgb::GuizmoManipulateCommand::GuizmoManipulateCommand(
	const std::vector<TransformMemento*>& _prevMementos,
	const std::vector<TransformMemento*>& _currMementos
)
	: prevMementos_ { _prevMementos }
	, currMementos_ { _currMementos }
{
}

mtgb::GuizmoManipulateCommand::~GuizmoManipulateCommand()
{
	SAFE_CLEAR_CONTAINER_DELETE(prevMementos_);
	SAFE_CLEAR_CONTAINER_DELETE(currMementos_);
}

void mtgb::GuizmoManipulateCommand::Execute() {}

void mtgb::GuizmoManipulateCommand::Undo()
{
	for (auto memento : prevMementos_)
	{
		Game::GetComponentFactory().AddComponentFromMemento(*memento);
	}
}

void mtgb::GuizmoManipulateCommand::Redo()
{
	for (auto memento : currMementos_)
	{
		Game::GetComponentFactory().AddComponentFromMemento(*memento);
	}
}

std::string mtgb::GuizmoManipulateCommand::Name() const
{
	return "GuizmoManipulatedEvent";
}