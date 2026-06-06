#pragma once
#include <mtgb.h>
#include <functional>
#include "UIComponent.h"

class Slider : public UIComponent
{
  public:
	Slider();
	void SetRect(const RectF& _rect);
	void SetLabel(std::string_view _label);
	void SetValue(int _value);
	void UpdateUI();

	void SetOnValueChanged(const std::function<void(int)>& _func)
	{
		onValueChanged_ = _func;
	}
	void OnEnable() override;
	void OnDisable() override;
	void OnSelected() override;
	void OnDeselected() override;

  private:
	void SlideValue(bool _isPositive);
	void UpdateSliderImage();
	RectF rect_;
	int minValue_;
	int maxValue_;
	int currValue_;
	float valueRatio_;
	std::function<void(int)> onValueChanged_;
	mtgb::ImageRenderer* pBackgroundRenderer_;
	mtgb::TextRenderer* pLabelRenderer_;

	mtgb::TextRenderer* pValueTextRenderer_;
	mtgb::ImageRenderer* pFillImageRenderer_;
	mtgb::ImageRenderer* pEmptyImageRenderer_;

	mtgb::ImageRenderer* pHandleImageRenderer_;
};