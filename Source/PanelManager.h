#pragma once
#include <mtgb.h>
#include <unordered_map>
#include "Panel.h"

class PanelManager
{
  public:
	PanelManager();
	void AddPanel(std::string_view _panelName, Panel* _pPanel);
	void EnablePanel(std::string_view _panelName);
	void MoveForcusForward();
	void MoveForcusBackward();
	void UpdatePanel();
  private:
	std::unordered_map<std::string, Panel*, mtgb::TransparentStringHash, mtgb::TransparentStringEq> panelMap_;
	Panel* pCurrPanel_;
};