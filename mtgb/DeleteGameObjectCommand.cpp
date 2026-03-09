#include "stdafx.h"
#include "DeleteGameObjectCommand.h"
#include "SceneSystem.h"
#include "EntityManager.h"
mtgb::DeleteGameObjectCommand::DeleteGameObjectCommand(
	GameObject* _pGameObj,
	const GameObjectFactory& _gameObjectFactory
)
	: gameObjectFactory_ { _gameObjectFactory }
	, targetEntityId_ { INVALID_ENTITY }
	, name_ { " Unknown" }
	, layerFlag_ { AllLayer() }
	, tag_ { GameObjectTag::UNTAGGED }
	, isNotCalledStart_ { false }
	, typeName_ { "Unknown" }
{
	if (_pGameObj == nullptr)
		return;

	name_	   = _pGameObj->GetName();
	layerFlag_ = _pGameObj->GetLayerFlag();
	tag_	   = _pGameObj->GetTag();
	typeName_  = _pGameObj->GetClassTypeName();

	targetEntityId_ = _pGameObj->GetEntityId();
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
		Game::GetComponentFactory().AddComponentFromMemento(*pMemento);
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
