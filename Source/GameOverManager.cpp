#include "stdafx.h"
#include "GameOverManager.h"
#include "ResultScene.h"
#include "GameEvents.h"

GameOverManager::GameOverManager()
	: GameObject{}
	, pFadeOutScreen_{nullptr}
	, fadeOutDuration_{3.0f}
	, startFadeOutDelay_{1.5f}
{
	pFadeOutScreen_ = Instantiate<FadeOutScreen>(fadeOutDuration_);

	// TODO: GameOverEventのような最終決定イベントを用意して、それだけ購読させる。
	// 何がゲームオーバーなのか判定する層を用意する

	// プレイヤーのHPが0になったイベントを購読、ゲームオーバーにする
	Game::System<EventManager>().GetEvent<PlayerHpReachedZeroEvent>().Subscribe(
		[this](const PlayerHpReachedZeroEvent& _event)
		{
			OnGameOver();
		},
		EventScope::Scene
	);
}

GameOverManager::~GameOverManager()
{
}

void GameOverManager::Update()
{
	if (pFadeOutScreen_->IsFinished())
	{
		Game::System<SceneSystem>().Move<ResultScene>();
	}
}

void GameOverManager::Start()
{
}

void GameOverManager::OnGameOver()
{
	Timer::AddAram(
		startFadeOutDelay_,
		[this]()
		{
			pFadeOutScreen_->StartFadeOut();
		}
	);
	Audio::PlayOneShotFile("Sound/GameOverSound.mp3");
}
