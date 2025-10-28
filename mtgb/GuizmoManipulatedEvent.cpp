#include "GuizmoManipulatedEvent.h"

mtgb::GuizmoManipulatedEvent::GuizmoManipulatedEvent(TransformMemento* _pPrevMemento, TransformMemento* _pMemento, const ComponentFactory& _componentFactory)
    : pPrevMemento_{ _pPrevMemento }
    , pMemento_{ _pMemento }
    , componentFactory_(_componentFactory)
{

}

void mtgb::GuizmoManipulatedEvent::Execute()
{

}

void mtgb::GuizmoManipulatedEvent::Undo()
{
    componentFactory_.AddComponentFromMemento(*pPrevMemento_);
}

void mtgb::GuizmoManipulatedEvent::Redo()
{
    componentFactory_.AddComponentFromMemento(*pMemento_);
}

std::string mtgb::GuizmoManipulatedEvent::Name() const
{
    return "GuizmoManipulatedEvent";
}
