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
		PropertyChangeCommand(IComponentMemento* _pPrevMemento, IComponentMemento* _pMemento);

		void Execute() override;
		void Undo() override;
		std::string Name() const override;

	  private:
		IComponentMemento* pPrevMemento_;
		IComponentMemento* pMemento_;
	};
} // namespace mtgb