#pragma once
#include "Command.h"
#include "Core/Component/IComponentMemento.h"
#include "Components/Transform/Transform.h"
#include "Core/Entity.h"
#include "Core/Component/ComponentFactory.h"
#include <typeindex>
#include <vector>

namespace mtgb
{
	/// <summary>
	/// エディタでギズモを操作した際のコマンド
	/// </summary>
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