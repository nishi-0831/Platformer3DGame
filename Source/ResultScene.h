#pragma once
#include "GameScene.h"
#include "PanelManager.h"

class ResultScene : public mtgb::GameScene
{
  public:
	ResultScene();
	~ResultScene();

	void Initialize() override;
	void Update() override;
	void Draw() const override;
	void End() override;

  private:
	void CreatePanel();
	PanelManager panelManager_;
	mtgb::TextRenderer* pScoreText_;
};