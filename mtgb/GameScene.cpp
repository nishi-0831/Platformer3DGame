#include "GameScene.h"
#include "SceneSystem.h"
#include "GameObject.h"
#include "Transform.h"
#include "CameraSystem.h"
#include "GameObjectTypeRegistry.h"
#include "EventManager.h"
#include "Editor/Command/SelectionCommand.h"
mtgb::GameScene::GameScene() {}

mtgb::GameScene::~GameScene()
{
	for (auto& pGameObject : pGameObjects_)
	{
		Game::RemoveEntityAllComponent(pGameObject->GetEntityId());
		pGameObject->DestroyMe(); // 削除フラグを立てておく
	}
	SAFE_CLEAR_CONTAINER_DELETE(pGameObjects_);
}

void mtgb::GameScene::RegisterGameObject(GameObject* _pGameObject)
{
	for (GameObject* pObj : pGameObjects_)
	{
		// 既に登録済みの場合は何もしない
		if (pObj->GetEntityId() == _pGameObject->GetEntityId())
			return;
	}
	pGameObjects_.push_back(_pGameObject);
}

mtgb::CameraHandleInScene mtgb::GameScene::RegisterCameraGameObject(GameObject* _pGameObject) const
{
	Transform* pTransform { &Transform::Get(_pGameObject->GetEntityId()) };
	return Game::System<CameraSystem>().RegisterDrawCamera(pTransform);
}

void mtgb::GameScene::Initialize() {}

void mtgb::GameScene::Update() {}

void mtgb::GameScene::Draw() const {}

void mtgb::GameScene::End() {}

mtgb::GameObject* mtgb::GameScene::GetGameObject(std::string_view _name) const
{
	for (auto& object : pGameObjects_)
	{
		if (object->GetName() != _name)
		{
			continue;
		}
		return object;
	}
	return nullptr;
}

mtgb::GameObject* mtgb::GameScene::GetGameObject(GameObjectTag _tag) const
{
	for (auto& object : pGameObjects_)
	{
		if (object->GetTag() == _tag)
		{
			return object;
		}
	}
	return nullptr;
}

void mtgb::GameScene::GetGameObjects(std::string_view _name, std::vector<GameObject*>* _pFoundGameObjects) const
{
	_pFoundGameObjects->clear();
	for (auto& object : pGameObjects_)
	{
		if (object->GetName() != _name)
		{
			continue;
		}
		_pFoundGameObjects->push_back(object);
	}
}

void mtgb::GameScene::GetGameObjects(GameObjectTag _tag, std::vector<GameObject*>* _pFoundGameObjects) const
{
	_pFoundGameObjects->clear();
	for (auto& object : pGameObjects_)
	{
		if (object->GetTag() != _tag)
		{
			continue;
		}
		_pFoundGameObjects->push_back(object);
	}
}

void mtgb::GameScene::GetAllGameObjects(std::list<GameObject*>* _gameObjects)
{
	*_gameObjects = pGameObjects_;
}

mtgb::GameObject* mtgb::GameScene::GetGameObject(EntityId _entityId) const
{
	for (auto& object : pGameObjects_)
	{
		if (object->GetEntityId() != _entityId)
		{
			continue;
		}
		return object;
	}

	return nullptr;
}

void mtgb::GameScene::DestroyGameObject(EntityId _entityId)
{
	if (_entityId == INVALID_ENTITY)
		return;

	for (auto& object : pGameObjects_)
	{
		if (object->GetEntityId() != _entityId)
		{
			continue;
		}
		object->DestroyMe();
	}
}

nlohmann::json mtgb::GameScene::SerializeGameObjects() const
{
	nlohmann::json j;
	// 配列として初期化
	j["GameObject"] = nlohmann::json::array();
	for (auto& object : pGameObjects_)
	{
		if (!object)
			continue;

		GameObjectTypeRegistry& gameObjTypeRegistry = Game::System<GameObjectTypeRegistry>();

		if (gameObjTypeRegistry.IsRegistered(mtgb::ExtractClassName(object->GetName())) == false)
			continue;

		nlohmann::json objJson = object->Serialize();
		j["GameObject"].push_back(objJson);
	}
	return j;
}

mtgb::GameScene* mtgb::GameScene::pInstance_ { nullptr };
