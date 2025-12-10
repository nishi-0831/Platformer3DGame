#include "GameObjectCreateCommand.h"
#include "SceneSystem.h"
#include "EntityManager.h"
#include "EventManager.h"
#include "GameObjectSelectionEvent.h"
mtgb::GameObjectCreateCommand::GameObjectCreateCommand( CreateFunc _createFunc, const ComponentFactory& _componentFactory)
    : createFunc_{_createFunc}
    , componentFactory_{_componentFactory}
    , json_{ nlohmann::json::object() }
    , deserialized_{false}
    , entityId_{INVALID_ENTITY}
{
}

mtgb::GameObjectCreateCommand::GameObjectCreateCommand(CreateFunc _createFunc, const ComponentFactory& _componentFactory, const nlohmann::json& _json)
    : GameObjectCreateCommand(_createFunc,_componentFactory)
{
    json_ = _json;
}

void mtgb::GameObjectCreateCommand::Execute()
{
    GameObject* obj = createFunc_();

    if (obj == nullptr)
        return;

    entityId_ = obj->GetEntityId();

    if (json_.is_null() || json_.empty())
    {
        gameObjectName_ = obj->GetName();
    }
    else if (deserialized_ == false)
    {
        Deserialize(obj);
        deserialized_ = true;
    }
    Game::System<EventManager>().GetEvent<GameObjectCreatedEvent>().Invoke({ .entityId = obj->GetEntityId() });
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

EntityId mtgb::GameObjectCreateCommand::GetCommandTargetEntityId() const
{
    return entityId_;
}



void mtgb::GameObjectCreateCommand::Deserialize(GameObject* _obj)
{
    _obj->Deserialize(json_);
}
