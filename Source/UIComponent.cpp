#include "UIComponent.h"
#include "Panel.h"
UIComponent::UIComponent()
	: isSelected_ { false }
	, pPanel_ { nullptr }
{
	isInspectable_ = false;
}

UIComponent::~UIComponent()
{
	if (pPanel_)
	{
		pPanel_->RemoveUIComponent(this);
	}
}

void UIComponent::SetPanel(Panel* _pPanel)
{
	pPanel_ = _pPanel;
}
