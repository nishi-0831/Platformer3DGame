#pragma once
#include "Command.h"
#include "IComponentMemento.h"
#include "Transform.h"
#include "Entity.h"
#include "ComponentFactory.h"
#include <typeindex>
#include <vector>

namespace mtgb
{
	class GuizmoManipulateCommand : public Command
	{
	  public:
		GuizmoManipulateCommand(
			const std::vector<TransformMemento*>& _prevMementos,
			const std::vector<TransformMemento*>& _currMementos
		);
		~GuizmoManipulateCommand();
		void Execute() override;
		void Undo() override;
		void Redo() override;
		std::string Name() const override;

	  private:
		std::vector<TransformMemento*> prevMementos_;
		std::vector<TransformMemento*> currMementos_;
	};
} // namespace mtgb