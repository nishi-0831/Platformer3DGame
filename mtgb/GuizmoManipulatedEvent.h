#pragma once
#include "Command.h"
#include "IComponentMemento.h"
#include "Transform.h"
#include "Entity.h"
#include "ComponentFactory.h"
#include <typeindex>

namespace mtgb
{
	class GuizmoManipulatedEvent : public Command
	{
	public:
		GuizmoManipulatedEvent(
			TransformMemento* _pPrevMemento, 
			TransformMemento* _pMemento,
			const ComponentFactory& _componentFactory);

		void Execute() override;
		void Undo() override;
		void Redo() override;
		std::string Name() const override;
		// ComponentFactory��null�ł��邱�Ƃ����e���Ȃ�
		const ComponentFactory& componentFactory_;
	private:
		TransformMemento* pPrevMemento_;
		TransformMemento* pMemento_;
	};
}