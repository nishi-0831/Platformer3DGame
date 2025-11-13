#include "GuizmoManipulatedEvent.h"

mtgb::GuizmoManipulateCommand::GuizmoManipulateCommand(TransformMemento* _pPrevMemento, TransformMemento* _pMemento, const ComponentFactory& _componentFactory)
    : pPrevMemento_{ _pPrevMemento }
    , pMemento_{ _pMemento }
    , componentFactory_(_componentFactory)
{

}

void mtgb::GuizmoManipulateCommand::Execute()
{

}

void mtgb::GuizmoManipulateCommand::Undo()
{
    componentFactory_.AddComponentFromMemento(*pPrevMemento_);
}

void mtgb::GuizmoManipulateCommand::Redo()
{
    componentFactory_.AddComponentFromMemento(*pMemento_);
}

std::string mtgb::GuizmoManipulateCommand::Name() const
{
    return "GuizmoManipulatedEvent";
}

EntityId mtgb::GuizmoManipulateCommand::GetCommandTargetEntityId() const
{
    return pMemento_->GetEntityId();
}
