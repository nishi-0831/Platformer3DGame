#pragma once
#include "GameScene.h"

class TitleScene : public mtgb::GameScene
{
public:
	TitleScene();
	~TitleScene();

	void Initialize() override;
	void Update() override;
	void Draw() const override;
	void End() override;
};