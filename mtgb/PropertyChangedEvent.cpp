#include "PropertyChangedEvent.h"

mtgb::PropertyChangeCommand::PropertyChangeCommand(IComponentMemento* _pPrevMemento, IComponentMemento* _pMemento, const ComponentFactory& _componentfactory)
    : pPrevMemento_{_pPrevMemento}
    , pMemento_{_pMemento}
    , componentFactory_{componentFactory_}
{
}

void mtgb::PropertyChangeCommand::Execute()
{
    componentFactory_.AddComponentFromMemento(*pMemento_);
}

void mtgb::PropertyChangeCommand::Undo()
{
    componentFactory_.AddComponentFromMemento(*pPrevMemento_);
}

std::string mtgb::PropertyChangeCommand::Name() const
{
    return "PropertyChanged";
}

EntityId mtgb::PropertyChangeCommand::GetCommandTargetEntityId() const
{
    return pMemento_->GetEntityId();
}
