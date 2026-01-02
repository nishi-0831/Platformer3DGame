#pragma once
#include "Command.h"
#include "IComponentMemento.h"
#include "Entity.h"
#include "ComponentFactory.h"
#include <typeindex>

namespace mtgb
{
	class PropertyChangeCommand : public Command
	{
	  public:
		PropertyChangeCommand(
			IComponentMemento* _pPrevMemento,
			IComponentMemento* _pMemento,
			const ComponentFactory& _componentfactory
		);

		void Execute() override;
		void Undo() override;
		std::string Name() const override;
		EntityId GetCommandTargetEntityId() const override;

	  private:
		// ComponentFactoryがnullであることを許容しない
		const ComponentFactory& componentFactory_;
		IComponentMemento* pPrevMemento_;
		IComponentMemento* pMemento_;
	};
} // namespace mtgb