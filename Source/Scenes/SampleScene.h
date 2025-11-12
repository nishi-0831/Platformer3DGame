#pragma once
#include "GameScene.h"
#include "../Source/StageID.h"
class SampleScene : public mtgb::GameScene
{
public:
	SampleScene();
	~SampleScene();

	void Initialize() override;
	void Update() override;
	void Draw() const override;
	void End() override;
private:
	StageID stageID_;
};
