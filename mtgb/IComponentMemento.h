#pragma once
#include "Entity.h"
#include <typeindex>
namespace mtgb
{
	class IComponentMemento
	{
	public:
		virtual ~IComponentMemento() = default;
		virtual std::type_index GetComponentType() const = 0;
		virtual EntityId GetEntityId() const = 0;
	};
}