#include "GuizmoManipulatedEvent.h"
#include "ReleaseUtility.h"

mtgb::GuizmoManipulateCommand::GuizmoManipulateCommand(
	TransformMemento* _pPrevMemento,
	TransformMemento* _pMemento
)
	: pPrevMemento_ { _pPrevMemento }
	, pMemento_ { _pMemento }
{
}

mtgb::GuizmoManipulateCommand::~GuizmoManipulateCommand()
{
	SAFE_DELETE(pPrevMemento_);
	SAFE_DELETE(pMemento_);
}

void mtgb::GuizmoManipulateCommand::Execute() {}

void mtgb::GuizmoManipulateCommand::Undo()
{
	Game::GetComponentFactory().AddComponentFromMemento(*pPrevMemento_);
}

void mtgb::GuizmoManipulateCommand::Redo()
{
	Game::GetComponentFactory().AddComponentFromMemento(*pMemento_);
}

std::string mtgb::GuizmoManipulateCommand::Name() const
{
	return "GuizmoManipulatedEvent";
}

mtgb::EntityId mtgb::GuizmoManipulateCommand::GetCommandTargetEntityId() const
{
	return pMemento_->GetEntityId();
}
