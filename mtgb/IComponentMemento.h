#pragma once
#include "Entity.h"
#include <typeindex>
#include "ComponentConcept.h"
namespace mtgb
{
	class IComponentMemento
	{
	public:
		virtual ~IComponentMemento() = default;
		virtual std::type_index GetComponentType() const = 0;
		virtual EntityId GetEntityId() const = 0;
	};

	template<typename TComponent,typename TState>
	class ComponentMemento : public IComponentMemento
	{
	public:
		using Component = TComponent;
		using State = TState;

		ComponentMemento(EntityId _entityId,const State& _state) 
			: entityId_{_entityId}
			, state_{ _state } 
		{
		}

		std::type_index GetComponentType() const  override { return typeid(TComponent); }
		EntityId GetEntityId() const override { return entityId_; }
		const State& GetState() const { return state_; }
	private:
		State state_;
		EntityId entityId_;
	};
}