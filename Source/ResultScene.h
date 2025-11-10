#pragma once
#include "GameScene.h"

class ResultScene : public mtgb::GameScene
{
public:
	ResultScene();
	~ResultScene();

	void Initialize() override;
	void Update() override;
	void Draw() const override;
	void End() override;
};