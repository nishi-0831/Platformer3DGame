#include "stdafx.h"
#include "DuplicateGameObjectCommand.h"
#include "Game.h"
#include "SceneSystem.h"
#include "ComponentRegistry.h"
#include "EntityManager.h"
#include <format>
mtgb::DuplicateGameObjectCommand::DuplicateGameObjectCommand(CreateFunc _createFunc, const ComponentFactory& _componentFactory, EntityId _srcEntityId)
    : createFunc_{_createFunc}
    , componentFactory_{_componentFactory}
    , srcEntityId_{_srcEntityId}
	, notSaveMementos_{true}
{
	GameObject* src = Game::System<SceneSystem>().GetActiveScene()->GetGameObject(srcEntityId_);
	if (src == nullptr)
		return;

	srcGameObjName_ = src->GetName();
	std::string classTypeName = src->GetClassTypeName();

	std::optional<std::vector<std::type_index>> componentPoolTypes = Game::System<ComponentRegistry>().GetComponentPoolTypes(srcEntityId_);

	if (componentPoolTypes.has_value() == false)
		return;

	componentPoolTypes_ = std::move(componentPoolTypes.value());
}

mtgb::DuplicateGameObjectCommand::~DuplicateGameObjectCommand()
{
	for (IComponentMemento* memento : mementos_)
	{
		SAFE_DELETE(memento);
	}
}

void mtgb::DuplicateGameObjectCommand::Execute()
{
	GameObject* dest = createFunc_();
	destGameObjName_ = dest->GetName();
	destEntityId_ = dest->GetEntityId();
	for (std::type_index componentPoolType : componentPoolTypes_)
	{
		IComponentPool* pComponentPool = Game::GetCP(componentPoolType);
		if (pComponentPool == nullptr)
			continue;

		pComponentPool->Copy(destEntityId_, srcEntityId_);
	}

	if (notSaveMementos_)
	{
		SaveToMementos();
		notSaveMementos_ = false;
	}
}

void mtgb::DuplicateGameObjectCommand::Undo()
{
	Game::System<SceneSystem>().GetActiveScene()->DestroyGameObject(destEntityId_);
}

void mtgb::DuplicateGameObjectCommand::Redo()
{
	Game::System<EntityManager>().DecrementCounter();

	GameObject* dest = createFunc_();

	for (IComponentMemento* memento : mementos_)
	{
		if (memento == nullptr)
			continue;

		componentFactory_.AddComponentFromMemento(*memento);
	}
}

std::string mtgb::DuplicateGameObjectCommand::Name() const
{
	return std::format("Duplicate {} from {}", destGameObjName_, srcGameObjName_);
}

mtgb::EntityId mtgb::DuplicateGameObjectCommand::GetCommandTargetEntityId() const
{
    return mtgb::EntityId();
}

void mtgb::DuplicateGameObjectCommand::SaveToMementos()
{
	for (std::type_index componentPoolType : componentPoolTypes_)
	{
		IComponentPool* pComponentPool = Game::GetCP(componentPoolType);
		if (pComponentPool == nullptr)
			continue;

		IComponentMemento* memento = pComponentPool->SaveToMemento(destEntityId_);
		if (memento == nullptr)
			continue;
		mementos_.push_back(memento);
	}
}
