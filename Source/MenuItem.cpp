#include "MenuItem.h"

ImageHandle Button::hImageOnSelected_ { mtgb::INVALID_HANDLE };

ImageHandle Button::hImageOnNotSelected_ { mtgb::INVALID_HANDLE };

Button::Button()
	: pImageRenderer_ { Component<mtgb::ImageRenderer>() }
	, pTextRenderer_ { Component<mtgb::TextRenderer>() }
	, onPressed_ { nullptr }
{
	Button::hImageOnSelected_	 = mtgb::Image::Load("Image/OnSelected.png");
	Button::hImageOnNotSelected_ = mtgb::Image::Load("Image/OnNotSelected.png");
	pTextRenderer_->alignment	 = mtgb::TextAlignment::CENTER;
	pImageRenderer_->handle_	 = Button::hImageOnNotSelected_;
}

void Button::SetRect(const RectF& _rect)
{
	rect_					   = _rect;
	pImageRenderer_->drawRect_ = rect_;
	pTextRenderer_->rect_	   = rect_;
}

void Button::OnPressed()
{
	onPressed_();
}

void Button::SetText(std::string_view _text)
{
	pTextRenderer_->text_ = _text;
}

void Button::OnSelected()
{
	isSelected_				 = true;
	pImageRenderer_->handle_ = hImageOnSelected_;
}

void Button::OnDeselected()
{
	isSelected_				 = false;
	pImageRenderer_->handle_ = hImageOnNotSelected_;
}

void Button::UpdateUI()
{
	if (mtgb::InputUtil::GetKeyDown(KeyCode::ENTER) || mtgb::InputUtil::GetGamePadDown(PadCode::CIRCLE))
	{
		onPressed_();
	}
}

void Button::OnEnable()
{
	pImageRenderer_->enabled_ = true;
	pTextRenderer_->enabled_  = true;
}

void Button::OnDisable()
{
	pImageRenderer_->enabled_ = false;
	pTextRenderer_->enabled_  = false;
}