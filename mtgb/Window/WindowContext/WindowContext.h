#pragma once
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

namespace mtgb
{
	enum class WindowContext
	{
		FIRST,
		SECOND,
		BOTH,
		NONE
	};
	WindowContext CurrContext();

} // namespace mtgb
