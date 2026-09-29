#include "stdafx.h"
#include "DeleteGameObjectCommand.h"
#include "Core/SceneSystem.h"
#include "Core/EntityManager.h"

mtgb::DeleteGameObjectCommand::DeleteGameObjectCommand(
	std::span<EntityId> _entityIds,
	const GameObjectFactory& _gameObjectFactory
)
	: gameObjectFactory_ { _gameObjectFactory }
{
	for (EntityId id : _entityIds)
	{
		GameObject* pGameObj = Game::System<SceneSystem>().GetActiveScene()->GetGameObject(id);
		if (pGameObj == nullptr)
			continue;

		GameObjectSnapshot snapshot { .entityId		  = id,
									  .name			  = pGameObj->GetName(),
									  .typeName		  = pGameObj->GetClassTypeName(),
									  .propertiesJson = pGameObj->SerializeProperties() };

		std::vector<IComponentMemento*> mementos;
		// IDに割り当てられているコンポーネントプールの型情報を取得
		std::optional<std::vector<std::type_index>> cpTypes =
			Game::System<ComponentRegistry>().GetComponentPoolTypes(id);
		if (cpTypes.has_value() == false)
			return;

		for (std::type_index cpType : cpTypes.value())
		{
			// コンポーネントプールのインターフェース取得
			IComponentPool* pComponentPool = Game::GetCP(cpType);
			if (pComponentPool == nullptr)
				continue;

			// メメントを作成
			IComponentMemento* pMemento = pComponentPool->SaveToMemento(id);
			if (pMemento == nullptr)
				continue;

			mementos.push_back(pMemento);
		}

		snapshot.mementos = mementos;

		snapshots_.push_back(snapshot);
	}
}

void mtgb::DeleteGameObjectCommand::Execute()
{
	for (const auto& snapshot : snapshots_)
	{
		Game::System<SceneSystem>().GetActiveScene()->MarkGameObjectPendingDestroy(snapshot.entityId);
	}
}

void mtgb::DeleteGameObjectCommand::Undo()
{
	for (const auto& snapshot : snapshots_)
	{
		GameObject* pGameObj = gameObjectFactory_.Create(snapshot.typeName);
		if (pGameObj == nullptr)
			continue;
		pGameObj->SetName(snapshot.name);
		pGameObj->DeserializeProperties(snapshot.propertiesJson);
		for (IComponentMemento* pMemento : snapshot.mementos)
		{
			Game::GetComponentFactory().AddComponentFromMemento(*pMemento);
		}
	}
}

std::string mtgb::DeleteGameObjectCommand::Name() const
{
	std::string str = "Delete->\n";
	for (auto& snapshots : snapshots_)
	{
		str += std::to_string(snapshots.entityId) + "\n";
	}
	return str;
}