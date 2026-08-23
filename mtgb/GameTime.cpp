#include "GameTime.h"
#include "IncludingWindows.h"
#include "Game.h"
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
	massert(QueryPerformanceCounter(&current_) == TRUE && "QueryPerformanceCounterの取得に失敗");
	deltaTime_ = static_cast<double>(current_.QuadPart - previous_.QuadPart) / frequency_.QuadPart;

	previous_ = current_;
	if (deltaTime_ < 0.0)
		deltaTime_ = 0.0;

	frameCount_++;
	elapsed_ += deltaTime_;

	if (elapsed_ * targetFrameRate_ >= 1.0)
	{
		if (waitFrame_ != 0)
		{
			waitFrame_--;
		}
		else
		{
			Game::UpdateFrame();
		}
	}
	if (elapsed_ >= 1.0)
	{
		currentFps_ = frameCount_;
		frameCount_ = 0;
		elapsed_	= 0.0;
		if (deltaTime_ > MAX_DELTA_TIME)
		{
			deltaTime_ = MAX_DELTA_TIME;
		}
	}
}

double mtgb::Time::deltaTime_ {};
unsigned int mtgb::Time::waitFrame_ { 0 };
double mtgb::Time::targetFrameRate_ { 60.0 };