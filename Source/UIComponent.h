#pragma once
#include <mtgb.h>

class Panel;
class UIComponent : public GameObject
{
  public:
	UIComponent();
	virtual ~UIComponent();
	virtual void OnEnable() {};
	virtual void OnDisable() {};
	virtual void UpdateUI() {};
	virtual void OnSelected() {};
	virtual void OnDeselected() {};
	void SetPanel(Panel* _pPanel);

  protected:
	bool isSelected_;
	Panel* pPanel_;
};