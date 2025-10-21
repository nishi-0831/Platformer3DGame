#pragma once
#include <typeindex>
#include <type_traits>
#include <unordered_map>
#include <concepts>
#include <IComponent.h>
#include "Entity.h"
#include <functional>
namespace mtgb
{
	template <typename T>
	concept ComponentT = requires(T com, EntityId id)
	{
		{ T::Get(id) };
	};
	class ComponentFactory
	{
	public:
		template<typename ComponentT>
		void Register();

	private:
		//std::unordered_map<std::type_index,>
	};
}