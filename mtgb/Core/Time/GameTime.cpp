#include "GameTime.h"
#include "Window/IncludingWindows.h"
#include "Core/Game.h"
#include "Editor/MTImGui.h"
#pragma comment(lib, "Winmm.lib")

mtgb::Time::Time()
	: current_ {}
	, previous_ {}
	, frequency_ {}
	, currentFps_ { 0 }
	, frameCount_ { 0 }
	, elapsed_ { 0.0 }
{
}

mtgb::Time::~Time()
{
	timeEndPeriod(1);
}

void mtgb::Time::Initialize()
{
	QueryPerformanceFrequency(&frequency_);
	QueryPerformanceCounter(&previous_);
	timeBeginPeriod(1);
}

void mtgb::Time::Update()
{
	// フレーム数加算
	frameCount_++;

	massert(QueryPerformanceCounter(&current_) == TRUE && "QueryPerformanceCounterの取得に失敗");

	// 設定したFPSの1フレームあたりの経過時間の目標値
	double targetFrameTime = 1.0 / targetFrameRate_;

	// フレーム間の経過時間
	double frameElapsed = (static_cast<double>(current_.QuadPart - previous_.QuadPart) / frequency_.QuadPart);
	// 経過時間を累積
	elapsed_ += frameElapsed;

	// 経過時間が目標値を超えた場合
	if (frameElapsed > targetFrameTime)
	{
		previous_ = current_;

		// デルタタイム更新
		deltaTime_ = frameElapsed;
		if (deltaTime_ > MAX_DELTA_TIME)
		{
			deltaTime_ = targetFrameTime;
		}
		if (deltaTime_ < 0.0)
		{
			deltaTime_ = 0.0;
		}

		// 待機フレーム数が設定されていた場合
		if (waitFrame_ != 0)
		{
			// 待機フレーム数を1減算して、ゲームの更新はしない
			waitFrame_--;
		}
		else
		{
			// ゲームの更新
			Game::UpdateFrame();
			//
			MTImGui::DirectShow(
				[this]()
				{
					ImGui::Text("FPS: %i", currentFps_);
					ImGui::Text("Frame Count: %i", frameCount_);
					ImGui::Text("DeltaD: %lf", deltaTime_);
				},
				"Time",
				ShowType::SETTINGS
			);
		}
	}
	// 1秒経過時
	if (elapsed_ >= 1.0)
	{
		currentFps_ = 1.0 / deltaTime_;
		frameCount_ = 0;
		elapsed_	= 0.0;
	}
}

double mtgb::Time::deltaTime_ {};
unsigned int mtgb::Time::waitFrame_ { 0 };
double mtgb::Time::targetFrameRate_ { 60.0 };