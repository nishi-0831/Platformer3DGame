#include "Panel.h"

Panel::Panel()
	: currIdx_ {-1}
{}

void Panel::IncrementIndex()
{
	MoveCurrIndex(1);
}

void Panel::DecrementIndex() 
{
	MoveCurrIndex(-1);
}

void Panel::Press() 
{
	if (currIdx_ < 0 || currIdx_ >= menuItems_.size())
		massert(false && "currIdx_が不正");

	menuItems_[currIdx_]->OnPressed();
}

void Panel::AddMenuItem(MenuItem* _pMenuItem) 
{
	if (_pMenuItem == nullptr)
	{
		assert(false && "_pMenuItemがnullptr");
		return;
	}

	menuItems_.push_back(_pMenuItem);
	if (currIdx_ < 0)
	{
		currIdx_ = menuItems_.size() - 1;
		menuItems_[currIdx_]->OnSelected();
	}

}

void Panel::Enable()
{
	for (MenuItem* item : menuItems_)
	{
		item->Enable();
	}
}

void Panel::Disable()
{
	for (MenuItem* item : menuItems_)
	{
		item->Disable();
	}
}

void Panel::MoveCurrIndex(int _movement) 
{
	menuItems_[currIdx_]->OnDeselected();
	currIdx_ += _movement;
	currIdx_ = (currIdx_ + menuItems_.size()) % menuItems_.size();
	menuItems_[currIdx_]->OnSelected();
}
