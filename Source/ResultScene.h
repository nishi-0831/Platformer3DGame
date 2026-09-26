#pragma once
#include <Core/GameScene.h>

#include "PanelManager.h"

class ResultScene : public mtgb::GameScene
{
  public:
	ResultScene();
	~ResultScene();

	void Initialize() override;
	void UpdateScene() override;
	void Draw() const override;

  private:
	void CreatePanel();
	PanelManager panelManager_;
	mtgb::TextRenderer* pScoreText_;
};