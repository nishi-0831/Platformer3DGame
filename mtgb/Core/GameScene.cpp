#include "GameScene.h"
#include "Core/SceneSystem.h"
#include "Core/GameObject/GameObject.h"
#include "Components/Transform/Transform.h"
#include "Camera/CameraSystem.h"
#include "GameObject/GameObjectTypeRegistry.h"
#include "EventManager.h"
#include "Editor/Command/SelectionCommand.h"
#include "EntityManager.h"
mtgb::GameScene::GameScene() {}

mtgb::GameScene::~GameScene()
{
	for (auto itr = pGameObjects_.begin(); itr != pGameObjects_.end();)
	{
		Game::RemoveEntityAllComponent((*itr)->GetEntityId());
		(*itr)->DestroyMe();
		SAFE_DELETE(*itr);
		itr = pGameObjects_.erase(itr);
		Game::System<EntityManager>().DecrementCounter();
	}
}

void mtgb::GameScene::RegisterGameObject(GameObject* _pGameObject)
{
	for (GameObject* obj : pGameObjects_)
	{
		// 既に登録済みの場合は何もしない
		if (obj->GetEntityId() == _pGameObject->GetEntityId())
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

void mtgb::GameScene::Update()
{
	if (Game::IsEditMode() == false)
	{
		// 更新処理
		UpdateScene();
		for (auto obj : pGameObjects_)
		{
			if (obj->IsNotCalledStart())
			{
				obj->Start();
				obj->MarkAsCalledStart();
			}
			obj->Update();
		}
	}
	else
	{
		for (auto obj : pGameObjects_)
		{
			if (obj->IsNotCalledStartOnEditMode())
			{
				obj->StartOnEditMode();
				obj->MarkAsCalledStartOnEditMode();
			}
		}
	}
}

void mtgb::GameScene::Draw() const {}

void mtgb::GameScene::End() {}

void mtgb::GameScene::UpdateScene() {}

void mtgb::GameScene::OnPreDrawGameObjects()
{
	for (auto obj : pGameObjects_)
	{
		obj->OnPreDrawScene();
	}
}

mtgb::GameObject* mtgb::GameScene::GetGameObject(std::string_view _name) const
{
	for (auto obj : pGameObjects_)
	{
		if (obj->GetName() != _name)
		{
			continue;
		}
		return obj;
	}
	return nullptr;
}

mtgb::GameObject* mtgb::GameScene::GetGameObject(GameObjectTag _tag) const
{
	for (auto obj : pGameObjects_)
	{
		if (obj->GetTag() == _tag)
		{
			return obj;
		}
	}
	return nullptr;
}

void mtgb::GameScene::GetGameObjects(std::string_view _name, std::vector<GameObject*>* _pFoundGameObjects) const
{
	_pFoundGameObjects->clear();
	for (auto obj : pGameObjects_)
	{
		if (obj->GetName() != _name)
		{
			continue;
		}
		_pFoundGameObjects->push_back(obj);
	}
}

void mtgb::GameScene::GetGameObjects(GameObjectTag _tag, std::vector<GameObject*>* _pFoundGameObjects) const
{
	_pFoundGameObjects->clear();
	for (auto obj : pGameObjects_)
	{
		if (obj->GetTag() != _tag)
		{
			continue;
		}
		_pFoundGameObjects->push_back(obj);
	}
}

void mtgb::GameScene::GetAllGameObjects(std::list<GameObject*>* _gameObjects)
{
	*_gameObjects = pGameObjects_;
}

mtgb::GameObject* mtgb::GameScene::GetGameObject(EntityId _entityId) const
{
	for (auto obj : pGameObjects_)
	{
		if (obj->GetEntityId() != _entityId)
		{
			continue;
		}
		return obj;
	}

	return nullptr;
}

void mtgb::GameScene::MarkGameObjectPendingDestroy(EntityId _entityId)
{
	if (_entityId == INVALID_ENTITY)
		return;

	for (auto obj : pGameObjects_)
	{
		if (obj->GetEntityId() != _entityId)
		{
			continue;
		}
		obj->DestroyMe();
	}
}

nlohmann::json mtgb::GameScene::SerializeGameObjects() const
{
	nlohmann::json j;
	// 配列として初期化
	j["GameObject"] = nlohmann::json::array();
	for (auto obj : pGameObjects_)
	{
		if (!obj)
			continue;

		GameObjectTypeRegistry& gameObjTypeRegistry = Game::System<GameObjectTypeRegistry>();

		if (gameObjTypeRegistry.IsRegistered(mtgb::ExtractClassName(obj->GetName())) == false)
			continue;
		obj->OnPreSave();
		nlohmann::json objJson = obj->Serialize();
		j["GameObject"].push_back(objJson);
	}
	return j;
}

void mtgb::GameScene::DestroyMarkedGameObjects()
{
	// 削除処理
	for (auto itr = pGameObjects_.begin(); itr != pGameObjects_.end();)
	{
		if ((*itr)->IsToDestroy())
		{
			Game::RemoveEntityAllComponent((*itr)->GetEntityId());
			SAFE_DELETE(*itr);
			itr = pGameObjects_.erase(itr);
			Game::System<EntityManager>().DecrementCounter();
		}
		else
		{
			itr++;
		}
	}
}
