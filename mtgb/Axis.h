#pragma once
#include <cstdint>
namespace mtgb
{
	enum struct Axis : uint8_t
	{
		X,
		Y,
		Z
	};

	enum struct PadAxisCode : uint8_t
	{
		LEFT_TRIGGER,
		RIGHT_TRIGGER,
		/*LeftStick,
		RightStick*/
	};

	enum struct FlightStickAxisCode : uint8_t
	{
		SLIDER
	};
} // namespace mtgb