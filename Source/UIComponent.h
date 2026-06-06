#pragma once
#include <mtgb.h>
class UIComponent : public GameObject
{
  public:
	UIComponent();

	virtual void OnEnable() {};
	virtual void OnDisable() {};
	virtual void UpdateUI() {};
	virtual void OnSelected() {};
	virtual void OnDeselected() {};

  protected:
	bool isSelected_;
};