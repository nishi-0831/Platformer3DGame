#pragma once
#include "Core/Game.h"

#include "Command.h"
#include "ReleaseUtility.h"
#include "ComponentConcept.h"
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
			Game::System<PoolType>().Remove(pMemento_->GetEntityId());
		}
		void Redo() override
		{
			T& component = T::Get(pMemento_->GetEntityId());
			component.RestoreFromMemento(*pMemento_);
		}
		std::string Name() const override
		{
			return std::string("AddComponent:") + T::TypeName();
		}

	  private:
		T::Memento* pMemento_;
	};
	template <IComponentWithMemento T>
	inline AddComponentCommandGeneric<T>::AddComponentCommandGeneric(T& _component)
		: pMemento_ { _component.SaveToMemento() }
	{
	}
	template <IComponentWithMemento T> inline AddComponentCommandGeneric<T>::~AddComponentCommandGeneric()
	{
		SAFE_DELETE(pMemento_);
	}
} // namespace mtgb