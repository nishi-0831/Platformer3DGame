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

	template<typename TComponent,typename TData>
	class ComponentMemento : public IComponentMemento
	{
	public:
		using Component = TComponent;
		using Data = TData;

		ComponentMemento(EntityId _entityId,const Data& _data) 
			: entityId_{_entityId}
			, data_{ _data } 
		{
		}

		std::type_index GetComponentType() const  override { return typeid(TComponent); }
		EntityId GetEntityId() const override { return entityId_; }
		const Data& GetData() const { return data_; }
	private:
		Data data_;
		EntityId entityId_;
	};
}