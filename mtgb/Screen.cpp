#include "Screen.h"
#include <string>
#include "ProfileUtlity.h"
#include "WindowManager.h"
#include "Game.h"
#include "WindowContext.h"
mtgb::Screen::Screen()
	: sizeRatio_ { 1.0f, 1.0f }
	, initialSize_ { 0, 0 }
	, size_ { 0, 0 }
	, fpsLimit_ { 0 }
{
}

mtgb::Screen::~Screen() {}

void mtgb::Screen::Initialize()
{
	// スクリーンの情報を取得
	initialSize_.x = ProfileInt::Load().Section("SCREEN").Param("Width").InitValue(800).Get();
	// initialSize_.x   = 1920;
	// initialSize_.y   = 1080;
	initialSize_.y = ProfileInt::Load().Section("SCREEN").Param("Height").InitValue(600).Get();
	fpsLimit_	   = ProfileInt::Load().Section("GAME").Param("Fps").InitValue(60).Get();

	size_ = initialSize_;
}

void mtgb::Screen::Update() {}

const mtgb::Vector2F mtgb::Screen::GetSizeRatio() const
{
	return sizeRatio_;
}

const mtgb::Vector2Int mtgb::Screen::GetInitialSize() const
{
	return initialSize_;
}

void mtgb::Screen::SetSize(int _width, int _height)
{
	size_.x = _width;
	size_.y = _height;

	sizeRatio_ = { (static_cast<float>(size_.x) / initialSize_.x), static_cast<float>(size_.y) / initialSize_.y };
}

int mtgb::Screen::GetFPS() const
{
	return fpsLimit_;
}
