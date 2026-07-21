#pragma once
#include "GameScene.h"
class StageEditScene : public mtgb::GameScene
{
  public:
	StageEditScene();
	StageEditScene(const nlohmann::json& _stageData);
	~StageEditScene();

	void Initialize() override;
	void Update() override;
	void Draw() const override;
	void End() override;

  private:
	nlohmann::json stageData_;
};