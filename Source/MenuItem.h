#pragma once
#include <functional>
#include <string>
#include <type_traits>
#include <mtgb.h>
#include "UIComponent.h"
class Button : public UIComponent
{
  public:
	Button();

	void SetRect(const RectF& _rect);
	void OnPressed();
	void SetText(std::string_view _text);
	virtual void OnSelected();
	virtual void OnDeselected();
	template <typename T>
		requires std::is_invocable_v<T>
	void SetOnPressed(T&& _func)
	{
		onPressed_ = std::forward<T>(_func);
	}
	void UpdateUI() override;
	void OnEnable() override;
	void OnDisable() override;
  private:
	mtgb::ImageRenderer* pImageRenderer_;
	mtgb::TextRenderer* pTextRenderer_;
	std::function<void()> onPressed_;
	bool isSelected_;
	RectF rect_;
	static ImageHandle hImageOnSelected_;
	static ImageHandle hImageOnNotSelected_;
};