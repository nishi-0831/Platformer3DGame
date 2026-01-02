#include "stdafx.h"
#include "DeleteGameObjectCommand.h"
#include "SceneSystem.h"
#include "EntityManager.h"
mtgb::DeleteGameObjectCommand::DeleteGameObjectCommand(
	GameObject* pGameObj,
	const GameObjectFactory& _gameObjectFactory,
	const ComponentFactory& _componentFactory
)
	: componentFactory_{_componentFactory}
	, gameObjectFactory_{_gameObjectFactory}
	, targetEntityId_{INVALID_ENTITY}
	, name_{" Unknown"}
	, layerFlag_{AllLayer()}
	, tag_{GameObjectTag::Untagged}
	, isNotCalledStart_{false}
	, typeName_{"Unknown"}
{
	if (pGameObj == nullptr)
		return;

	name_	   = pGameObj->GetName();
	layerFlag_ = pGameObj->GetLayerFlag();
	tag_	   = pGameObj->GetTag();
	typeName_  = pGameObj->GetClassTypeName();

	targetEntityId_ = pGameObj->GetEntityId();
	// IDに割り当てられているコンポーネントプールの型情報を取得
	std::optional<std::vector<std::type_index>> cpTypes =
		Game::System<ComponentRegistry>().GetComponentPoolTypes(targetEntityId_);
	if (cpTypes.has_value() == false)
		return;

	for (std::type_index cpType : cpTypes.value())
	{
		// コンポーネントプールのインターフェース取得
		IComponentPool* pComponentPool = Game::GetCP(cpType);
		if (pComponentPool == nullptr)
			continue;

		// メメントを作成
		IComponentMemento* pMemento = pComponentPool->SaveToMemento(targetEntityId_);
		if (pMemento == nullptr)
			continue;

		mementos_.push_back(pMemento);
	}
}

void mtgb::DeleteGameObjectCommand::Execute()
{
	Game::System<SceneSystem>().GetActiveScene()->DestroyGameObject(targetEntityId_);
}

void mtgb::DeleteGameObjectCommand::Undo()
{
	Game::System<EntityManager>().DecrementCounter();
	GameObject* pGameObj = gameObjectFactory_.Create(typeName_);
	pGameObj->SetName(name_);
	// FIXME: アクセス制限により書き換えできないため、完全なUndoではない
	// pGameObj->layerFlag_ = layerFlag_;
	// pGameObj->tag_ = tag_;
	// pGameObj->isNotCalledStart_ = isNotCalledStart_;

	for (IComponentMemento* pMemento : mementos_)
	{
		componentFactory_.AddComponentFromMemento(*pMemento);
	}
}

std::string mtgb::DeleteGameObjectCommand::Name() const
{
	return std::format("delete {},EntityId {}", name_, targetEntityId_);
}

mtgb::EntityId mtgb::DeleteGameObjectCommand::GetCommandTargetEntityId() const
{
	return targetEntityId_;
}
