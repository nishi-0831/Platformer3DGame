#include "AddComponentCommand.h"
#include "Core/Component/IComponentPool.h"
#include "Core/Component/ComponentRegistry.h"
#include "Core/Game.h"
mtgb::AddComponentCommand::AddComponentCommand(
	mtgb::EntityId _entityId,
	std::type_index _typeIdx,
	mtgb::IComponentMemento* _pMemento
)
	: entityId_ { _entityId }
	, componentType_ { _typeIdx }
	, pMemento_ { _pMemento }
{
}

mtgb::AddComponentCommand::~AddComponentCommand()
{
	SAFE_DELETE(pMemento_);
}

void mtgb::AddComponentCommand::Execute()
{
	ApplyMemento();
}

void mtgb::AddComponentCommand::Undo()
{
	RemoveComponent();
}

void mtgb::AddComponentCommand::ApplyMemento()
{
	if (pMemento_ == nullptr)
	{
		// entityIdから既存のコンポーネントを取得、なければ新規作成しMementoに保存する
		pMemento_ = Game::GetComponentFactory().AddComponent(componentType_, entityId_);
	}
	else
	{
		// Mementoからコンポーネントを復元
		Game::GetComponentFactory().AddComponentFromMemento(*pMemento_);
	}
}

void mtgb::AddComponentCommand::RemoveComponent() const
{
	std::optional<std::type_index> componentPoolType =
		Game::System<ComponentRegistry>().GetComponentPoolType(componentType_);
	if (componentPoolType.has_value())
	{
		Game::RemoveEntityComponent(componentPoolType.value(), entityId_);
	}
}

std::string mtgb::AddComponentCommand::Name() const
{
	return std::string("AddComponent:") + componentType_.name();
}