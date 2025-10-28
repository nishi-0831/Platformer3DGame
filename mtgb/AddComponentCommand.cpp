#include "AddComponentCommand.h"
#include "IComponentPool.h"
mtgb::AddComponentCommand::AddComponentCommand(
    mtgb::EntityId _entityId,
    const std::type_index& _typeIdx,
    mtgb::IComponentMemento* _memento,
    const mtgb::ComponentFactory& _componentFactory)
    : entityId_(_entityId)
    , componentType_(_typeIdx)
    , memento_(_memento)
    , componentFactory_(_componentFactory)
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
   
    if (memento_ == nullptr)
    {
        // entityIdから既存のコンポーネントを取得、なければ新規作成しMementoに保存する
        memento_ = componentFactory_.AddComponent(componentType_, entityId_);
    }
    else
    {
        // Mementoからコンポーネントを復元
        componentFactory_.AddComponentFromMemento(*memento_);
    }
}

void mtgb::AddComponentCommand::RemoveComponent() const
{
    std::optional<std::type_index> componentPoolType = IComponentPool::GetComponentPoolType(componentType_);
    if (componentPoolType.has_value())
    {
        Game::RemoveEntityComponent(componentPoolType.value(), entityId_);
    }
}

std::string mtgb::AddComponentCommand::Name() const
{
    return std::string("AddComponent:" ) +componentType_.name();
}
