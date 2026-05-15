#include "MenuItem.h"

ImageHandle MenuItem::hImageOnSelected_ { mtgb::INVALID_HANDLE };

ImageHandle MenuItem::hImageOnNotSelected_ { mtgb::INVALID_HANDLE };

MenuItem::MenuItem()
	: pImageRenderer_ { Component<mtgb::ImageRenderer>() }
	, pTextRenderer_ { Component<mtgb::TextRenderer>() }
	, onPressed_ {nullptr}
	, isSelected_ {false}
{
	MenuItem::hImageOnSelected_ = mtgb::Image::Load("Image/OnSelected.png");
	MenuItem::hImageOnNotSelected_ = mtgb::Image::Load("Image/OnNotSelected.png");
	pTextRenderer_->alignment = mtgb::TextAlignment::CENTER;
	pImageRenderer_->handle_	   = MenuItem::hImageOnNotSelected_;
}

void MenuItem::SetRect(const RectF& _rect)
{
	rect_ = _rect;
	pImageRenderer_->drawRect_ = rect_;
	pTextRenderer_->rect_	   = rect_;
}

void MenuItem::OnPressed()
{
	onPressed_();
}

void MenuItem::SetText(std::string_view _text) 
{
	pTextRenderer_->text_ = _text;
}

void MenuItem::OnSelected() 
{
	isSelected_ = true;
	pImageRenderer_->handle_ = hImageOnSelected_;
}

void MenuItem::OnDeselected() 
{
	isSelected_				 = false;
	pImageRenderer_->handle_ = hImageOnNotSelected_;
}

void MenuItem::Enable()
{
	pImageRenderer_->enabled_ = true;
	pTextRenderer_->enabled_  = true;
}

void MenuItem::Disable()
{
	pImageRenderer_->enabled_ = false;
	pTextRenderer_->enabled_  = false;
}