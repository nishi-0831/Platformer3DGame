#include "AddComponentCommand.h"

mtgb::AddComponentCommand::AddComponentCommand(
    mtgb::EntityId _entityId,
    const std::type_index& _typeIdx,
    mtgb::IComponentMemento* _memento,
    mtgb::ComponentFactory* _componentFactory)
    : entityId_(_entityId)
    , componentType_(_typeIdx)
    , memento_(_memento)
    , pComponentFactory_(_componentFactory)
{
}

void mtgb::AddComponentCommand::Execute()
{
    ApplyMemento();
}

void mtgb::AddComponentCommand::Undo()
{
    RemoveComponent();
}



void mtgb::AddComponentCommand::ApplyMemento() 
{
    if (pComponentFactory_ == nullptr)
        return;

    if (memento_ == nullptr)
    {
        memento_ = pComponentFactory_->AddComponent(componentType_, entityId_);
    }
    else
    {
        // Memento����R���|�[�l���g�𕜌�
        pComponentFactory_->AddComponentFromMemento(*memento_);
    }
}

void mtgb::AddComponentCommand::RemoveComponent() const
{
    Game::RemoveEntityComponent(entityId_);
}

std::string mtgb::AddComponentCommand::Name() const
{
    return "AddComponent";
}
