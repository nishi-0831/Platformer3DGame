#pragma once
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

namespace mtgb
{
	enum class WindowContext
	{
		First,
		Second,
		Both,
		None
	};
	WindowContext CurrContext();
	
}
