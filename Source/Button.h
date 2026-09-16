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
	/// <summary>
	/// ボタンが押されたときに呼ばれる
	/// </summary>
	void OnPressed();
	/// <summary>
	/// ボタンのテキストを設定
	/// </summary>
	/// <param name="_text"></param>
	void SetText(std::string_view _text);
	virtual void OnSelected();
	virtual void OnDeselected();
	/// <summary>
	/// ボタンを押された時のコールバックを設定する
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <param name="_func"></param>
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
	RectF rect_;
	/// <summary>
	/// ボタンが選択されているときの画像
	/// </summary>
	static ImageHandle hImageOnSelected_;
	/// <summary>
	/// ボタンが選択されていないときの画像
	/// </summary>
	static ImageHandle hImageOnNotSelected_;
	static unsigned int generateCounter_;
};