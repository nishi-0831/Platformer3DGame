#include "stdafx.h"
#include "DuplicateGameObjectCommand.h"
#include "Core/Game.h"
#include "Core/SceneSystem.h"
#include "Core/Component/ComponentRegistry.h"
#include "Core/EntityManager.h"
#include <format>
mtgb::DuplicateGameObjectCommand::DuplicateGameObjectCommand(
	std::span<EntityId> _entityIds,
	const GameObjectFactory& _gameObjectFactory
)
	: gameObjectFactory_ { _gameObjectFactory }
{
	for (EntityId id : _entityIds)
	{
		DuplicateSnapshot snapshot;
		snapshot.entityId = id;
		std::optional<std::vector<std::type_index>> componentPoolTypes =
			Game::System<ComponentRegistry>().GetComponentPoolTypes(id);

		if (componentPoolTypes.has_value() == false)
			continue;
		snapshot.componentPoolTypes = std::move(componentPoolTypes.value());

		snapshot.typeName = Game::System<SceneSystem>().GetActiveScene()->GetGameObject(id)->GetClassTypeName();

		snapshots_.emplace_back(snapshot);
	}
}
mtgb::DuplicateGameObjectCommand::~DuplicateGameObjectCommand()
{
	for (auto& snapshot : snapshots_)
	{
		for (IComponentMemento* memento : snapshot.mementos)
		{
			SAFE_DELETE(memento);
		}
	}
}

void mtgb::DuplicateGameObjectCommand::Execute()
{
	for (auto& snapshot : snapshots_)
	{
		GameObject* dest	   = gameObjectFactory_.Create(snapshot.typeName);
		snapshot.destEntityId_ = dest->GetEntityId();

		for (std::type_index componentPoolType : snapshot.componentPoolTypes)
		{
			IComponentPool* pComponentPool = Game::GetCP(componentPoolType);
			if (pComponentPool == nullptr)
				continue;

			pComponentPool->Copy(snapshot.destEntityId_, snapshot.entityId);
		}

		if (snapshot.notSaveMementos)
		{
			SaveToMementos(snapshot);
			snapshot.notSaveMementos = false;
		}

		// GameObjectとその派生クラスのメンバ変数をコピーする

		GameObject* srcGameObj = Game::System<SceneSystem>().GetActiveScene()->GetGameObject(snapshot.entityId);

		// 複製したゲームオブジェクト名を保持。名前はコピーさせない。
		std::string destObjName = dest->GetName();

		if (srcGameObj != nullptr)
		{
			nlohmann::json srcPropertiesJson = srcGameObj->SerializeProperties();
			dest->DeserializeProperties(srcPropertiesJson);

			// ゲームオブジェクト名を戻す
			dest->SetName(destObjName);
		}
	}
}

void mtgb::DuplicateGameObjectCommand::Undo()
{
	for (auto& snapshot : snapshots_)
	{
		Game::System<SceneSystem>().GetActiveScene()->MarkGameObjectPendingDestroy(snapshot.destEntityId_);
	}
}

void mtgb::DuplicateGameObjectCommand::Redo()
{
	for (auto& snapshot : snapshots_)
	{
		GameObject* dest = gameObjectFactory_.Create(snapshot.typeName);
		for (IComponentMemento* memento : snapshot.mementos)
		{
			if (memento == nullptr)
				continue;
			Game::GetComponentFactory().AddComponentFromMemento(*memento);
		}

		// GameObjectとその派生クラスのメンバ変数をコピーする

		GameObject* srcGameObj = Game::System<SceneSystem>().GetActiveScene()->GetGameObject(snapshot.entityId);

		// 複製したゲームオブジェクト名を保持。名前はコピーさせない。
		std::string destObjName = dest->GetName();

		if (srcGameObj != nullptr)
		{
			nlohmann::json srcPropertiesJson = srcGameObj->SerializeProperties();
			dest->DeserializeProperties(srcPropertiesJson);

			// ゲームオブジェクト名を戻す
			dest->SetName(destObjName);
		}
	}
}

std::string mtgb::DuplicateGameObjectCommand::Name() const
{
	std::string str = "Duplicate->\n";
	for (auto& snapshot : snapshots_)
	{
		str += std::format("src: {},dest: {} \n", snapshot.entityId, snapshot.destEntityId_);
	}
	return str;
}

void mtgb::DuplicateGameObjectCommand::SaveToMementos(DuplicateSnapshot& _snapshot)
{
	for (std::type_index componentPoolType : _snapshot.componentPoolTypes)
	{
		IComponentPool* pComponentPool = Game::GetCP(componentPoolType);
		if (pComponentPool == nullptr)
			continue;

		IComponentMemento* memento = pComponentPool->SaveToMemento(_snapshot.destEntityId_);
		if (memento == nullptr)
			continue;
		_snapshot.mementos.push_back(memento);
	}
}
