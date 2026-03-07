#pragma once
#include "Game.h"

#include "Command.h"
#include "IComponentMemento.h"
#include "Entity.h"
#include "ComponentFactory.h"
#include "ComponentConcept.h"
#include <typeindex>
#include <string>

namespace mtgb
{
	template <IComponentWithMemento T> class AddComponentCommandGeneric : public Command
	{
	  public:
		AddComponentCommandGeneric(T& _component);
		~AddComponentCommandGeneric();
		// Command を介して継承されました
		void Execute() override {}
		void Undo() override
		{
			using PoolType = typename T::Pool;
			Game::System<PoolType>().Remove(memento_->GetEntityId());
		}
		void Redo() override
		{
			T* pComponent = T::Reuse(memento_->GetEntityId());
			assert(pComponent != nullptr);
			pComponent->RestoreFromMemento(*memento_);
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
	template <IComponentWithMemento T>
	inline AddComponentCommandGeneric<T>::AddComponentCommandGeneric(T& _component)
		: memento_ { _component.SaveToMemento() }
	{
	}
	template <IComponentWithMemento T> inline AddComponentCommandGeneric<T>::~AddComponentCommandGeneric()
	{
		SAFE_DELETE(memento_);
	}
} // namespace mtgb