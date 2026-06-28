#pragma once
#include <mtgb.h>
#include "FadeOutScreen.h"

/// <summary>
/// ゲームオーバー時の処理を管理するゲームオブジェクト
/// </summary>
class GameOverManager : public mtgb::GameObject
{
  public:
	GameOverManager();
	~GameOverManager();
	void Update() override;
	void Start() override;

  private:
	void OnGameOver();
	// ゲームオーバー時のフェードアウトを行うゲームオブジェクト
	FadeOutScreen* pFadeOutScreen_;
	// フェードアウトの秒数
	float fadeOutDuration_;
	// ゲームオーバーになってからフェードアウトを行うまでの猶予
	float startFadeOutDelay_;
};