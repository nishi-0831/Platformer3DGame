#pragma once
#include <mtgb.h>
#include "UIComponent.h"

class Slider : public UIComponent
{
  public:
	Slider();
	void SetRect(const RectF& _rect);
	void SetLabel(std::string_view _label);
	void UpdateUI();

  private:
	void SlideValue(bool _isPositive);
	void UpdateSliderImage();
	RectF rect_;
	int minValue_;
	int maxValue_;
	int currValue_;
	float valueRatio_;
	mtgb::ImageRenderer* pBackgroundRenderer_;
	mtgb::TextRenderer* pLabelRenderer_;

	mtgb::TextRenderer* pValueTextRenderer_;
	mtgb::ImageRenderer* pFillImageRenderer_;
	mtgb::ImageRenderer* pEmptyImageRenderer_;
};