#include "Slider.h"
#include <cmath>
Slider::Slider()
	: minValue_ { 10 }
	, maxValue_ { 180 }
	, currValue_ { 60 }
	, valueRatio_ { 0.1f }
	, pImageRenderer_ { Component<mtgb::ImageRenderer>() }
	, pTextRenderer_ { Component<mtgb::TextRenderer>() }
{
	pTextRenderer_->text_ = std::to_string(currValue_);
}

void Slider::SetRect(const RectF& _rect)
{
	rect_				  = _rect;
	pTextRenderer_->rect_ = _rect;
}

void Slider::UpdateUI()
{
	using namespace mtgb;
	if (InputUtil::GetKeyDown(KeyCode::LEFT) ||
		InputUtil::GetStickDown(Axis::X, StickType::LEFT, StickDirection::Negative))
	{
		SlideValue(false);
	}
	if (InputUtil::GetKeyDown(KeyCode::RIGHT) ||
		InputUtil::GetStickDown(Axis::X, StickType::LEFT, StickDirection::Positive))
	{
		SlideValue(true);
	}
}

void Slider::SlideValue(bool _isPositive)
{
	int valueRange		  = maxValue_ - minValue_;
	float increasingValue = std::roundf(valueRange * valueRatio_);
	int dir				  = _isPositive ? 1 : -1;
	currValue_ += static_cast<int>(increasingValue) * dir;
	currValue_			  = std::clamp(currValue_, minValue_, maxValue_);
	pTextRenderer_->text_ = std::to_string(currValue_);
}
