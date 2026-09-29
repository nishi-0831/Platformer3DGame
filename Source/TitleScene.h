#pragma once
#include <Core/GameScene.h>

#include "PanelManager.h"

class TitleScene : public mtgb::GameScene
{
  public:
	TitleScene();
	~TitleScene();

	void Initialize() override;
	void UpdateScene() override;
	void Draw() const override;
	void End() override;

  private:
	void CreatePanel();
	PanelManager panelManager_;
};