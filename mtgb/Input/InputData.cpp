#define NOMINMAX
#include "InputData.h"
void mtgb::InputData::Initialize()
{
	// 入力状態を初期化

	// キーボード
	keyStateCurrent_.reset();
	keyStatePrevious_.reset();

	// マウス
	mouseStateCurrent_	= _DIMOUSESTATE {};
	mouseStatePrevious_ = _DIMOUSESTATE {};

	// コントローラー
	joyStateCurrent_  = DIJOYSTATE {};
	joyStatePrevious_ = DIJOYSTATE {};

	// 傾いてない状態に設定
	for (auto& state : stickTiltStates_)
	{
		state.isFullyTiltedCurr_ = false;
		state.isFullyTiltedPrev_ = false;
	}
}
