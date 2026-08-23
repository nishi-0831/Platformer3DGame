#pragma once
#include <mtgb.h>
#include <functional>
#include "UIComponent.h"

class Slider : public UIComponent
{
  public:
	Slider();
	void Start() override;
	/// <summary>
	/// スライダーの描画矩形を設定
	/// </summary>
	/// <param name="_rect"></param>
	void SetRect(const RectF& _rect);
	/// <summary>
	/// スライダーのラベル設定
	/// </summary>
	/// <param name="_label"></param>
	void SetLabel(std::string_view _label);
	/// <summary>
	/// スライダーの値を設定
	/// </summary>
	/// <param name="_value"></param>
	void SetValue(int _value);
	void UpdateUI();
	/// <summary>
	/// スライダーの値が変更された時に呼ばれるコールバックを設定
	/// </summary>
	/// <param name="_func"></param>
	void SetOnValueChanged(const std::function<void(int)>& _func)
	{
		onValueChanged_ = _func;
	}
	void OnEnable() override;
	void OnDisable() override;
	void OnSelected() override;
	void OnDeselected() override;

  private:
	/// <summary>
	/// スライダーの値を変化させる
	/// </summary>
	/// <param name="_isPositive">trueなら正方向(右)、falseなら負方向(左)</param>
	void SlideValue(bool _isPositive);
	void UpdateSliderImage();
	// スライダーの描画矩形
	RectF rect_;
	// 最小値
	int minValue_;
	// 最大値
	int maxValue_;
	// 現在の値
	int currValue_;
	// スライダーの値を増減させる際の割合。0.1なら1割りずつ
	float valueRatio_;
	std::function<void(int)> onValueChanged_;
	// 背景
	mtgb::ImageRenderer* pBackgroundRenderer_;
	// ラベル
	mtgb::TextRenderer* pLabelRenderer_;
	// スライダーの値
	mtgb::TextRenderer* pValueTextRenderer_;
	// スライダーの最小値から最大値のうち、現在の値が満たしている範囲の画像
	mtgb::ImageRenderer* pFillImageRenderer_;
	// スライダーの最小値から最大値のうち、現在の値が満たしていない範囲の画像
	mtgb::ImageRenderer* pEmptyImageRenderer_;
	// スライダーの現在の値を示す画像
	mtgb::ImageRenderer* pHandleImageRenderer_;

	static unsigned int generateCounter_;
};