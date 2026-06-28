#include "Slider.h"
#include <cmath>

unsigned int Slider::generateCounter_ { 0 };

Slider::Slider()
	: minValue_ { 10 }
	, maxValue_ { 180 }
	, currValue_ { 60 }
	, valueRatio_ { 0.1f }
	, pBackgroundRenderer_ { Component<mtgb::ImageRenderer>() }
	, pLabelRenderer_ { Component<mtgb::TextRenderer>() }
	, onValueChanged_ { nullptr }
	, pValueTextRenderer_ { nullptr }
	, pFillImageRenderer_ { nullptr }
	, pEmptyImageRenderer_ { nullptr }
	, pHandleImageRenderer_ { nullptr }
{
	using namespace mtgb;

	std::string typeName = Game::System<GameObjectTypeRegistry>().GetNameFromType(typeid(Slider));
	name_				 = std::format("{} ({})", typeName, generateCounter_++);

	pLabelRenderer_->alignment	   = TextAlignment::CENTER;
	pLabelRenderer_->params_.depth = 3;

	GameObject* fillArea = new GameObject();
	Game::System<SceneSystem>().GetActiveScene()->RegisterGameObject(fillArea);
	pFillImageRenderer_					 = fillArea->Component<ImageRenderer>();
	pFillImageRenderer_->handle_		 = Image::Load("Image/FillArea.png");
	pFillImageRenderer_->uiParams_.depth = 2;

	GameObject* emptyArea = new GameObject();
	Game::System<SceneSystem>().GetActiveScene()->RegisterGameObject(emptyArea);

	pValueTextRenderer_				   = emptyArea->Component<TextRenderer>();
	pValueTextRenderer_->text_		   = std::to_string(currValue_);
	pValueTextRenderer_->alignment	   = TextAlignment::CENTER;
	pValueTextRenderer_->params_.depth = 3;

	pEmptyImageRenderer_				  = emptyArea->Component<ImageRenderer>();
	pEmptyImageRenderer_->handle_		  = Image::Load("Image/EmptyArea.png");
	pEmptyImageRenderer_->uiParams_.depth = 1;
	pBackgroundRenderer_->handle_		  = Image::Load("Image/Black.png");
	pBackgroundRenderer_->uiParams_.depth = 0;

	GameObject* handle = new GameObject();
	Game::System<SceneSystem>().GetActiveScene()->RegisterGameObject(handle);
	pHandleImageRenderer_				   = handle->Component<ImageRenderer>();
	pHandleImageRenderer_->handle_		   = Image::Load("Image/Circle.png");
	pHandleImageRenderer_->uiParams_.depth = 4;
}

void Slider::Start()
{
	SetRect(pBackgroundRenderer_->drawRect_);
}

void Slider::SetRect(const RectF& _rect)
{
	rect_							= _rect;
	pBackgroundRenderer_->drawRect_ = _rect;
	pValueTextRenderer_->rect_		= _rect;
	const float SLIDER_RECT_RATIO { 0.7f };
	float ratioWidth = rect_.width * SLIDER_RECT_RATIO;

	pEmptyImageRenderer_->drawRect_.x	   = _rect.x;
	pEmptyImageRenderer_->drawRect_.width  = ratioWidth;
	pEmptyImageRenderer_->drawRect_.y	   = _rect.y;
	pEmptyImageRenderer_->drawRect_.height = _rect.height;
	pValueTextRenderer_->rect_			   = pEmptyImageRenderer_->drawRect_;
	pFillImageRenderer_->drawRect_		   = pValueTextRenderer_->rect_;

	pLabelRenderer_->rect_.x	  = rect_.x + ratioWidth;
	pLabelRenderer_->rect_.width  = rect_.width - ratioWidth;
	pLabelRenderer_->rect_.y	  = _rect.y;
	pLabelRenderer_->rect_.height = _rect.height;

	pHandleImageRenderer_->drawRect_.width	= _rect.height / 2;
	pHandleImageRenderer_->drawRect_.height = _rect.height / 2;
	pHandleImageRenderer_->drawRect_.x		= pFillImageRenderer_->drawRect_.x + pFillImageRenderer_->drawRect_.width -
										 (pHandleImageRenderer_->drawRect_.width / 2);
	pHandleImageRenderer_->drawRect_.y = _rect.y + (pHandleImageRenderer_->drawRect_.height / 2);

	UpdateSliderImage();
}

void Slider::SetLabel(std::string_view _label)
{
	pLabelRenderer_->text_ = _label;
}

void Slider::SetValue(int _value)
{
	currValue_				   = std::clamp(_value, minValue_, maxValue_);
	pValueTextRenderer_->text_ = std::to_string(currValue_);
	UpdateSliderImage();
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

void Slider::OnEnable()
{
	pBackgroundRenderer_->enabled_	= true;
	pLabelRenderer_->enabled_		= true;
	pValueTextRenderer_->enabled_	= true;
	pFillImageRenderer_->enabled_	= true;
	pEmptyImageRenderer_->enabled_	= true;
	pHandleImageRenderer_->enabled_ = true;
}

void Slider::OnDisable()
{
	pBackgroundRenderer_->enabled_	= false;
	pLabelRenderer_->enabled_		= false;
	pValueTextRenderer_->enabled_	= false;
	pFillImageRenderer_->enabled_	= false;
	pEmptyImageRenderer_->enabled_	= false;
	pHandleImageRenderer_->enabled_ = false;
}

void Slider::OnSelected()
{
	pHandleImageRenderer_->enabled_ = true;
}

void Slider::OnDeselected()
{
	pHandleImageRenderer_->enabled_ = false;
}

void Slider::SlideValue(bool _isPositive)
{
	int valueRange		  = maxValue_ - minValue_;
	float increasingValue = std::roundf(valueRange * valueRatio_);
	int dir				  = _isPositive ? 1 : -1;
	currValue_ += static_cast<int>(increasingValue) * dir;
	currValue_				   = std::clamp(currValue_, minValue_, maxValue_);
	pValueTextRenderer_->text_ = std::to_string(currValue_);

	UpdateSliderImage();
	onValueChanged_(currValue_);
}

void Slider::UpdateSliderImage()
{
	float percentage					 = static_cast<float>(currValue_ - minValue_) / (maxValue_ - minValue_);
	pFillImageRenderer_->drawRect_.width = pEmptyImageRenderer_->drawRect_.width * percentage;
	pHandleImageRenderer_->drawRect_.x	 = pFillImageRenderer_->drawRect_.x + pFillImageRenderer_->drawRect_.width -
										 (pHandleImageRenderer_->drawRect_.width / 2);
}
