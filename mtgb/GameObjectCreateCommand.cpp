#include "GameObjectCreateCommand.h"
#include "SceneSystem.h"
#include "EntityManager.h"
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
        mementos_ = obj->GetDefaultMementos(entityId_);
    }
    else if (deserialized_ == false)
    {
        Deserialize(obj);
        deserialized_ = true;
    }

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

void mtgb::GameObjectCreateCommand::Deserialize(GameObject* _obj)
{
    auto mementos = Game::DeserializeComponents(entityId_, json_);
    if (mementos.has_value())
    {
        mementos_ = mementos.value();
    }

    _obj->Deserialize(json_);
}
