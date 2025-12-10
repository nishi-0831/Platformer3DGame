#pragma once
#include "Game.h"

#include "Command.h"
#include "IComponentMemento.h"
#include "Entity.h"
#include "ComponentFactory.h"
#include <typeindex>
#include <string>

namespace mtgb
{
	template<typename T>
	class AddComponentCommandGeneric : public Command
	{
	public:
		AddComponentCommandGeneric(T& _component);
		~AddComponentCommandGeneric();
		// Command ÇâÓÇµÇƒåpè≥Ç≥ÇÍÇ‹ÇµÇΩ
		void Execute() override
		{
		}
		void Undo() override
		{
			using PoolType = typename T::Pool;
			Game::System<PoolType>().Remove(memento_->GetEntityId());
		}
		void Redo() override
		{
			T& component = T::Get(memento_->GetEntityId());
			component.RestoreFromMemento(*memento_);
		}
		std::string Name() const override
		{
			return std::string("AddComponent:") + T::TypeName();
		}
		mtgb::EntityId GetCommandTargetEntityId() const override
		{
			return memento_->GetEntityId();
		}
	private:
		T::Memento* memento_;
	};
	template<typename T>
	inline AddComponentCommandGeneric<T>::AddComponentCommandGeneric(T& _component)
		: memento_{ _component.SaveToMemento() }
	{
		
	}
	template<typename T>
	inline AddComponentCommandGeneric<T>::~AddComponentCommandGeneric()
	{
		SAFE_DELETE(memento_);
	}
}