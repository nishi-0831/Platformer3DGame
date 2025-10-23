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

void mtgb::AddComponentCommand::ApplyMemento() 
{
	if (memento_ == nullptr || pComponentFactory_)
		return;

    // Mementoからコンポーネントを復元
    pComponentFactory_->AddComponentFromMemento(*memento_);
}

void mtgb::AddComponentCommand::RemoveComponent() const
{
    Game::RemoveEntityComponent(entityId_);
}
