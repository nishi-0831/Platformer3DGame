#include "InputConfig.h"
#include <algorithm>
void mtgb::InputConfig::SetRange(LONG _range)
{
	xRange = _range;
	yRange = _range;
	zRange = _range;
}

void mtgb::InputConfig::SetRange(LONG _range, Axis _axis)
{
	switch (_axis)
	{
		case Axis::X :
			xRange = _range;
			break;
		case Axis::Y :
			yRange = _range;
			break;
		case Axis::Z :
			zRange = _range;
			break;
	}
}

void mtgb::InputConfig::SetDeadZone(float _deadZone)
{
	deadZone = _deadZone;
}

float mtgb::InputConfig::ApplyDeadZone(const float _value) const
{
	// 絶対値がデッドゾーンに満たないなら0
	if (std::abs(_value) < deadZone)
	{
		return 0.0f;
	}
	// 0～1の範囲に正規化する
	float sign = (_value > 0) ? 1.0f : -1.0f;
	return sign * (std::abs(_value) - deadZone) / (1.0f - deadZone);
}