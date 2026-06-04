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
	void SlideValue(bool _isPositive);
	RectF rect_;
	int minValue_;
	int maxValue_;
	int currValue_;
	float valueRatio_;
	mtgb::ImageRenderer* pImageRenderer_;
	mtgb::TextRenderer* pTextRenderer_;
};