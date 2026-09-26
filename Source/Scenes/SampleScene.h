#pragma once
#include <Core/GameScene.h>
#include "../Source/StageID.h"
class SampleScene : public mtgb::GameScene
{
  public:
	SampleScene();
	SampleScene(const nlohmann::json& _stageData);
	~SampleScene();

	void Initialize() override;
	void UpdateScene() override;
	void Draw() const override;
	void End() override;

  private:
	StageID stageID_;
	nlohmann::json stageData_;
};
