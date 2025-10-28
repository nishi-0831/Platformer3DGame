#include "PropertyChangedEvent.h"

mtgb::PropertyChangedEvent::PropertyChangedEvent(IComponentMemento* _pPrevMemento, IComponentMemento* _pMemento, const ComponentFactory& _componentfactory)
    : pPrevMemento_{_pPrevMemento}
    , pMemento_{_pMemento}
    , componentFactory_{componentFactory_}
{
}

void mtgb::PropertyChangedEvent::Execute()
{
    componentFactory_.AddComponentFromMemento(*pMemento_);
}

void mtgb::PropertyChangedEvent::Undo()
{
    componentFactory_.AddComponentFromMemento(*pPrevMemento_);
}

std::string mtgb::PropertyChangedEvent::Name() const
{
    return "PropertyChanged";
}
