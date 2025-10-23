#pragma once
#include <concepts>
#include "Entity.h"
#include "IComponentMemento.h"
#include <type_traits>
namespace mtgb
{
	template <typename T>
	concept ComponentT = requires(T _com, EntityId _id)
	{
		T::Get(_id);
	};

	template <typename T,typename M>
	concept MementoT = 
		ComponentT<T> &&
		std::derived_from<M,IComponentMemento> &&
		requires(T& _com,const M& _memento)
	{
		_com.RestoreFromMemento(_memento);
	};
}