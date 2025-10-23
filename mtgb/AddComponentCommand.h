#pragma once
#include "Command.h"
#include "IComponentMemento.h"
#include "Entity.h"
#include "ComponentFactory.h"
#include <typeindex>

namespace mtgb
{
	/// <summary>
	/// コンポーネント追加コマンド
	/// </summary>
	class AddComponentCommand : public Command
	{
		AddComponentCommand(
			EntityId _entityId,
			const std::type_index& _typeIdx,
			IComponentMemento* _memento,
			ComponentFactory* _pComponentFactory);
		~AddComponentCommand() = default;
	private:
		EntityId entityId_;
		std::type_index componentType_;
		IComponentMemento* memento_;
		ComponentFactory* pComponentFactory_;

		void ApplyMemento();
		void RemoveComponent() const;
	};
}