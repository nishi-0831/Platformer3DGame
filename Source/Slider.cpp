#include "Slider.h"
#include <cmath>
Slider::Slider()
	: minValue_ { 10 }
	, maxValue_ { 180 }
	, currValue_ { 60 }
	, valueRatio_ { 0.1f }
	, pBackgroundRenderer_ { Component<mtgb::ImageRenderer>() }
	, pLabelRenderer_ { Component<mtgb::TextRenderer>() }
	, pValueTextRenderer_ {nullptr}
	, pFillImageRenderer_ {nullptr}
	, pEmptyImageRenderer_ {nullptr}

{
	using namespace mtgb;
	pLabelRenderer_->alignment = TextAlignment::CENTER;
	pLabelRenderer_->params_.depth = 3;

	GameObject* fillArea = new GameObject();
	Game::System<SceneSystem>().GetActiveScene()->RegisterGameObject(fillArea);
	pFillImageRenderer_ = fillArea->Component<ImageRenderer>();
	pFillImageRenderer_->handle_ = Image::Load("Image/FillArea.png");
	pFillImageRenderer_->uiParams_.depth = 2;

	GameObject* emptyArea		 = new GameObject();
	pValueTextRenderer_	  = emptyArea->Component<TextRenderer>();
	pValueTextRenderer_->text_ = std::to_string(currValue_);
	pValueTextRenderer_->alignment = TextAlignment::CENTER;
	pValueTextRenderer_->params_.depth = 3;

	pEmptyImageRenderer_		 = emptyArea->Component<ImageRenderer>();
	pEmptyImageRenderer_->handle_ = Image::Load("Image/EmptyArea.png");
	pEmptyImageRenderer_->uiParams_.depth = 1;
	pBackgroundRenderer_->handle_ = Image::Load("Image/Black.png");
	pBackgroundRenderer_->uiParams_.depth = 0;
	
}

void Slider::SetRect(const RectF& _rect)
{
	rect_				  = _rect;
	pBackgroundRenderer_->drawRect_ = _rect;
	pValueTextRenderer_->rect_ = _rect;
	const float SLIDER_RECT_RATIO { 0.7f };
	float ratioWidth = rect_.width * SLIDER_RECT_RATIO;

	pEmptyImageRenderer_->drawRect_.x = _rect.x;
	pEmptyImageRenderer_->drawRect_.width = ratioWidth;
	pEmptyImageRenderer_->drawRect_.y	  = _rect.y;
	pEmptyImageRenderer_->drawRect_.height	  = _rect.height;
	pValueTextRenderer_->rect_			  = pEmptyImageRenderer_->drawRect_;
	pFillImageRenderer_->drawRect_			  = pValueTextRenderer_->rect_;

	pLabelRenderer_->rect_.x = rect_.x + ratioWidth;
	pLabelRenderer_->rect_.width =  rect_.width - ratioWidth;
	pLabelRenderer_->rect_.y	 = _rect.y;
	pLabelRenderer_->rect_.height	 = _rect.height;

	UpdateSliderImage();
}

void Slider::SetLabel(std::string_view _label) 
{
	pLabelRenderer_->text_ = _label;
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
	pValueTextRenderer_->text_ = std::to_string(currValue_);
	
	UpdateSliderImage();
}

void Slider::UpdateSliderImage() 
{
	float percentage					 = static_cast<float>(currValue_ - minValue_) / (maxValue_ - minValue_);
	pFillImageRenderer_->drawRect_.width = pEmptyImageRenderer_->drawRect_.width * percentage;
}
