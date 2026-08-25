#pragma once
#include <mtgb.h>
#include <unordered_map>
#include "Panel.h"

class PanelManager
{
  public:
	PanelManager();
	/// <summary>
	/// 指定した名前でパネルを追加
	/// </summary>
	/// <param name="_panelName"></param>
	/// <param name="_pPanel"></param>
	void AddPanel(std::string_view _panelName, Panel* _pPanel);
	/// <summary>
	/// 指定した名前のパネルを有効化する
	/// </summary>
	/// <param name="_panelName"></param>
	void EnablePanel(std::string_view _panelName);
	/// <summary>
	/// UIのフォーカスを前方に移動する。
	/// パネルに追加した順番でフォーカスが切り替わる
	/// </summary>
	void MoveFocusForward();
	/// <summary>
	/// UIのフォーカスを後方に移動する。
	/// パネルに追加した順番でフォーカスが切り替わる
	/// </summary>
	void MoveFocusBackward();
	void UpdatePanel();

  private:
	std::unordered_map<std::string, Panel*, mtgb::TransparentStringHash, mtgb::TransparentStringEq> panelMap_;
	Panel* pCurrPanel_;
};