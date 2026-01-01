#pragma once
#include <mtgb.h>
#include "FadeOutScreen.h"
class GameOverManager : public mtgb::GameObject
{
  public:
	GameOverManager();
	~GameOverManager();
	void Update() override;
	void Start() override;

  private:
	void OnGameOver();
	FadeOutScreen* pFadeOutScreen_;
	float fadeOutDuration_;
	float startFadeOutDelay_;
};