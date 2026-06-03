#include "Slider.h"

Slider::Slider()
	: minValue_ {10}
	, maxValue_ {180}
	, currValue_ {60}
{}

void Slider::UpdateUI()
{
	using namespace mtgb;
	if (InputUtil::GetKeyDown(KeyCode::LEFT) || InputUtil::GetStickDown(Axis::X, StickType::LEFT, StickDirection::Negative))
	{
		
	}
	if (InputUtil::GetKeyDown(KeyCode::LEFT) ||
		InputUtil::GetStickDown(Axis::X, StickType::LEFT, StickDirection::Negative))
	{
	}
}
