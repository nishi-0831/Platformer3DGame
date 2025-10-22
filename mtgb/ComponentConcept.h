#pragma once
#include <concepts>
#include "Entity.h"
namespace mtgb
{
	template <typename T>
	concept ComponentT = requires(T _com, EntityId _id)
	{
		{ T::Get(_id) };
	};
}