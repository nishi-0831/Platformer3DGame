#pragma once
#include "Command.h"
#include "IComponentMemento.h"
#include "Transform.h"
#include "Entity.h"
#include "ComponentFactory.h"
#include <typeindex>

namespace mtgb
{
	class GuizmoManipulateCommand : public Command
	{
	  public:
		GuizmoManipulateCommand(
			TransformMemento* _pPrevMemento,
			TransformMemento* _pMemento
		);
		~GuizmoManipulateCommand();
		void Execute() override;
		void Undo() override;
		void Redo() override;
		std::string Name() const override;
		EntityId GetCommandTargetEntityId() const override;

	  private:
		TransformMemento* pPrevMemento_;
		TransformMemento* pMemento_;
	};
} // namespace mtgb