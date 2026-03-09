#include "GameObjectCreateCommand.h"
#include "SceneSystem.h"
#include "EntityManager.h"
#include "EventManager.h"
#include "GameObjectSelectionEvent.h"

void mtgb::GameObjectCreateCommand::Execute()
{
	CreateGameObject();

	Game::System<EventManager>().GetEvent<GameObjectCreatedEvent>().Invoke({ .entityId = entityId_ });
}

void mtgb::GameObjectCreateCommand::Undo()
{
	Game::System<SceneSystem>().GetActiveScene()->DestroyGameObject(entityId_);
}

void mtgb::GameObjectCreateCommand::Redo()
{
	Game::System<EntityManager>().ChangeCounter(entityId_ - 1);

	CreateGameObject();
}

std::string mtgb::GameObjectCreateCommand::Name() const
{
	return std::string("CreateGameObject:") + gameObjectName_;
}

mtgb::EntityId mtgb::GameObjectCreateCommand::GetCommandTargetEntityId() const
{
	return entityId_;
}

void mtgb::GameObjectCreateCommand::CreateGameObject()
{
	GameObject* obj = createFunc_();

	if (obj == nullptr)
		return;

	entityId_ = obj->GetEntityId();

	if (json_.is_null() || json_.empty())
	{
		gameObjectName_ = obj->GetName();
	}
	else
	{
		Deserialize(obj);
	}
}

void mtgb::GameObjectCreateCommand::Deserialize(GameObject* _obj)
{
	_obj->Deserialize(json_);
}
