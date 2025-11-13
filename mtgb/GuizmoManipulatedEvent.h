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
			TransformMemento* _pMemento,
			const ComponentFactory& _componentFactory);

		void Execute() override;
		void Undo() override;
		void Redo() override;
		std::string Name() const override;
		EntityId GetCommandTargetEntityId() const override;
	private:
		// ComponentFactory‚ªnull‚Å‚ ‚é‚±‚Æ‚ð‹–—e‚µ‚È‚¢
		const ComponentFactory& componentFactory_;
		TransformMemento* pPrevMemento_;
		TransformMemento* pMemento_;
	};
}