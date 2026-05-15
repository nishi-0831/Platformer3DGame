#pragma once
#include <mtgb.h>
#include <vector>
#include "MenuItem.h"

class Panel : public GameObject
{
  public:
	Panel();
	void IncrementIndex();
	void DecrementIndex();
	void Press();
	void AddMenuItem(MenuItem* _pMenuItem);
	void Enable();
	void Disable();
  private:
	void MoveCurrIndex(int _movement);
	std::vector<MenuItem*> menuItems_;
	int currIdx_;
};