#pragma once
#include <functional>
#include <concepts>
#include <type_traits>

#include "Entity.h"

namespace mtgb
{
	/// <summary>
	/// EntityIdを引数にとるstd::functionのエイリアス
	/// </summary>
	using EntityCallback = std::function<void(EntityId _entityId)>;

	/// <summary>
	/// EntityIdを引数にとるstd::functionに変換可能か調べる
	/// </summary>
	template <typename Func>
	concept EntityCallable = std::is_convertible_v<Func, EntityCallback>;
} // namespace mtgb