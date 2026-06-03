#include "PanelManager.h"
#include <InputData.h>
PanelManager::PanelManager() 
	: pCurrPanel_ {nullptr}
{

}

void PanelManager::AddPanel(std::string_view _panelName, Panel* _pPanel) 
{
	if (panelMap_.contains(_panelName))
		return;
	if (_pPanel == nullptr)
	{
		massert(false && "_pPanelがnullptr");
		return;
	}

	panelMap_[std::string(_panelName)] = _pPanel;
	if (pCurrPanel_ == nullptr)
	{
		pCurrPanel_ = _pPanel;
		pCurrPanel_->Enable();
	}
	else
	{
		_pPanel->Disable();
	}
}

void PanelManager::EnablePanel(std::string_view _panelName) 
{
	auto itr = panelMap_.find(_panelName);

	if (itr == panelMap_.end())
	{
		LOGIMGUI("panel: %.*s not exist ", static_cast<int>(_panelName.size()), _panelName.data());
		return;
	}
	if (pCurrPanel_ != nullptr)
	{
		pCurrPanel_->Disable();
	}
	pCurrPanel_ = itr->second;
	pCurrPanel_->Enable();
}
void PanelManager::MoveForcusBackward()
{
	pCurrPanel_->IncrementIndex();
}

void PanelManager::MoveForcusForward()
{
	pCurrPanel_->DecrementIndex();
}

void PanelManager::UpdatePanel() 
{
	using namespace mtgb;
	if (InputUtil::GetKeyDown(KeyCode::DOWN) ||
		InputUtil::GetStickDown(mtgb::Axis::Y, StickType::LEFT, StickDirection::Positive))
	{
		MoveForcusBackward();
	}
	if (InputUtil::GetKeyDown(KeyCode::UP) ||
		InputUtil::GetStickDown(mtgb::Axis::Y, StickType::LEFT, StickDirection::Negative))
	{
		MoveForcusForward();
	}
	pCurrPanel_->UpdateUI();
}
