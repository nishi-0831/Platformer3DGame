#pragma once
#include "Command.h"
#include "Core/Component/IComponentMemento.h"
#include "Core/Entity.h"
#include "Core/Component/ComponentFactory.h"
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