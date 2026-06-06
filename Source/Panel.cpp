#include "Panel.h"
Panel::Panel()
	: currIdx_ { -1 }
{
}

void Panel::IncrementIndex()
{
	MoveCurrIndex(1);
}

void Panel::DecrementIndex()
{
	MoveCurrIndex(-1);
}

void Panel::UpdateUI() 
{
	if (currIdx_ < 0 || currIdx_ >= uiComponents_.size())
		massert(false && "currIdx_が不正");

	uiComponents_[currIdx_]->UpdateUI();
}

void Panel::AddUIComponent(UIComponent* _pUIComponent)
{
	if (_pUIComponent == nullptr)
	{
		assert(false && "_pUIComponentがnullptr");
		return;
	}

	uiComponents_.push_back(_pUIComponent);
	if (currIdx_ < 0)
	{
		currIdx_ = uiComponents_.size() - 1;
		uiComponents_[currIdx_]->OnSelected();
	}
}

void Panel::Enable()
{
	for (UIComponent* item : uiComponents_)
	{
		item->OnEnable();
	}
}

void Panel::Disable()
{
	for (UIComponent* item : uiComponents_)
	{
		item->OnDisable();
	}
}

void Panel::MoveCurrIndex(int _movement)
{
	mtgb::Game::System<mtgb::Audio>().Play("MoveCursor");

	uiComponents_[currIdx_]->OnDeselected();
	currIdx_ += _movement;
	currIdx_ = (currIdx_ + uiComponents_.size()) % uiComponents_.size();
	uiComponents_[currIdx_]->OnSelected();
}
