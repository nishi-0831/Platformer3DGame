#pragma once
#include "Command.h"
#include "IComponentMemento.h"
#include "Entity.h"
#include "ComponentFactory.h"
#include <typeindex>

namespace mtgb
{
	class PropertyChangedEvent : public Command
	{
	public:
		PropertyChangedEvent(
			IComponentMemento* _pPrevMemento,
			IComponentMemento* _pMemento,
			const ComponentFactory& _componentfactory);

		void Execute() override;
		void Undo() override;
		std::string Name() const override;
		// ComponentFactory‚ªnull‚Å‚ ‚é‚±‚Æ‚ð‹–—e‚µ‚È‚¢
		const ComponentFactory& componentFactory_;

	private:
		IComponentMemento* pPrevMemento_;
		IComponentMemento* pMemento_;
	};
}