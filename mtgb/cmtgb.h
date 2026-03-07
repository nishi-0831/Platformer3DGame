#pragma once
// MTGameBase Common includes, type alias and constant values

#include <cstdint>
// #include "MTAssert.h"
#include "Handlers.h"

namespace mtgb
{
	using EntityId = int64_t;						 // エンティティの識別番号
	inline constexpr EntityId INVALID_ENTITY { -1 }; // -1番は無効Id
} // namespace mtgb
