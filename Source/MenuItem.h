#pragma once
#include <functional>
#include <string>
#include <type_traits>
#include <mtgb.h>
class MenuItem : public GameObject
{
  public:
	MenuItem();
	void SetRect(const RectF& _rect);
	void OnPressed();
	void SetText(std::string_view _text);
	void OnSelected();
	void OnDeselected();
	template <typename T>
		requires std::is_invocable_v<T>
	void SetOnPressed(T&& _func)
	{
		onPressed_ = std::forward<T>(_func);
	}
	void Enable();
	void Disable();
  private:
	mtgb::ImageRenderer* pImageRenderer_;
	mtgb::TextRenderer* pTextRenderer_;
	std::function<void()> onPressed_;
	bool isSelected_;
	RectF rect_;
	static ImageHandle hImageOnSelected_;
	static ImageHandle hImageOnNotSelected_;
};