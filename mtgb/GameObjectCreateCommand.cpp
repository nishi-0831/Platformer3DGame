#include "GameObjectCreateCommand.h"
#include "SceneSystem.h"
#include "EntityManager.h"
mtgb::GameObjectCreateCommand::GameObjectCreateCommand( CreateFunc _createFunc, const ComponentFactory& _componentFactory)
    : createFunc_{_createFunc}
    , componentFactory_{_componentFactory}
{
}
void mtgb::GameObjectCreateCommand::Execute()
{
    GameObject* obj = createFunc_();

    if (obj == nullptr)
        return;

    entityId_ = obj->GetEntityId();
    gameObjectName_ = obj->GetName();
    mementos_ = obj->GetDefaultMementos(entityId_);
    ApplyComponents();
}

void mtgb::GameObjectCreateCommand::Undo()
{
    Game::System<SceneSystem>().GetActiveScene()->DestroyGameObject(entityId_);
}

void mtgb::GameObjectCreateCommand::Redo()
{
    Game::System<EntityManager>().DecrementCounter();

    Execute();
}

std::string mtgb::GameObjectCreateCommand::Name() const
{
    return  std::string("CreateGameObject:" ) + gameObjectName_;
}

void mtgb::GameObjectCreateCommand::ApplyComponents()
{
    for (IComponentMemento* memento : mementos_)
    {
        if (memento == nullptr)
            continue;

        componentFactory_.AddComponentFromMemento(*memento);
    }
}
