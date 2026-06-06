#pragma once
#include <mtgb.h>
#include <vector>
#include "UIComponent.h"

class Panel : public GameObject
{
  public:
	Panel();
	void IncrementIndex();
	void DecrementIndex();
	void UpdateUI();
	void AddUIComponent(UIComponent* _pUIComponent);
	void Enable();
	void Disable();
  private:
	void MoveCurrIndex(int _movement);
	std::vector<UIComponent*> uiComponents_;
	int currIdx_;
};