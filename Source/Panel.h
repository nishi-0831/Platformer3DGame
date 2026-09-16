#pragma once
#include <mtgb.h>
#include <vector>
#include "UIComponent.h"

/// <summary>
/// ボタンやスライダーなどのUIコンポーネントをまとめる
/// </summary>
class Panel : public GameObject
{
  public:
	Panel();
	/// <summary>
	/// インデックスをインクリメントして、有効なUIコンポーネントを変える
	/// </summary>
	void IncrementIndex();
	/// <summary>
	/// インデックスをデクリメントして、有効なUIコンポーネントを変える
	/// </summary>
	void DecrementIndex();
	void UpdateUI();
	/// <summary>
	/// UIコンポーネントを追加
	/// </summary>
	/// <param name="_pUIComponent"></param>
	void AddUIComponent(UIComponent* _pUIComponent);
	/// <summary>
	/// 指定したUIコンポーネントを除去
	/// </summary>
	/// <param name="_pUIComponent"></param>
	void RemoveUIComponent(UIComponent* _pUIComponent);
	void Enable();
	void Disable();

  private:
	void MoveCurrIndex(int _movement);
	std::vector<UIComponent*> uiComponents_;
	int currIdx_;
};