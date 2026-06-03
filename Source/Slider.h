#pragma once
#include <mtgb.h>
#include "UIComponent.h"

class Slider : public UIComponent
{
  public:
	Slider();
	void SetRect(const RectF& _rect);
	void UpdateUI();
  private:
	RectF rect_;
	int minValue_;
	int maxValue_;
	int currValue_;
	
};