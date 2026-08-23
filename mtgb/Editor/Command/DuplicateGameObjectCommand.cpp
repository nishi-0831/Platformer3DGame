#include "stdafx.h"
#include "DuplicateGameObjectCommand.h"
#include "Core/Game.h"
#include "SceneSystem.h"
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
	}
}

void mtgb::DuplicateGameObjectCommand::Undo()
{
	for (auto& snapshot : snapshots_)
	{
		Game::System<SceneSystem>().GetActiveScene()->DestroyGameObject(snapshot.destEntityId_);
	}
}

void mtgb::DuplicateGameObjectCommand::Redo()
{
	for (size_t i = 0; i < snapshots_.size(); i++)
	{
		Game::System<EntityManager>().DecrementCounter();
	}
	for (auto& snapshot : snapshots_)
	{
		gameObjectFactory_.Create(snapshot.typeName);
		for (IComponentMemento* memento : snapshot.mementos)
		{
			if (memento == nullptr)
				continue;
			Game::GetComponentFactory().AddComponentFromMemento(*memento);
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
