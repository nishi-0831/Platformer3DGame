#pragma once

#include <array>
#include <unordered_map>
#include "IncludingInput.h"
#include <bitset>

#include "Math/Vector3.h"
#include "Math/Vector2Int.h"
#include "Math/Vector2F.h"
#include "InputKeyCode.h"
#include "InputMouseButton.h"
#include "InputPadButton.h"

#include "cmtgb.h"
#include "Window/WindowContext/WindowContext.h"
#include "InputConfig.h"
#include "Input.h"

namespace mtgb
{
	class InputData final
	{
		friend class Input;
		friend class InputQuery;
		friend class InputResource;

	  private:
		void Initialize();
		static inline constexpr size_t KEY_COUNT { 256 }; // キーの数
		std::bitset<KEY_COUNT> keyStateCurrent_;		  // キーボードの状態現在
		std::bitset<KEY_COUNT> keyStatePrevious_;		  // キーボードの状態前回

		_DIMOUSESTATE mouseStateCurrent_;  // マウスの状態現在
		_DIMOUSESTATE mouseStatePrevious_; // マウスの状態前回
		DIJOYSTATE joyStateCurrent_;	   // コントローラーの状態現在
		DIJOYSTATE joyStatePrevious_;	   // コントローラーの状態現在
		GamePadType gamePadType_;		   // コントローラーの種類
		InputConfig config_;			   // 入力の取り方の設定

		Vector2Int mousePosition_; // マウスカーソルの座標

		/// <summary>
		/// スティックの傾き状態
		/// </summary>
		struct StickTiltState
		{
			bool isFullyTiltedPrev_; // スティックが完全に傾けられているか 前回
			bool isFullyTiltedCurr_; // スティックが完全に傾けられているか 現在
		};
		/// <summary>
		/// スティックの数
		/// </summary>
		static constexpr size_t STICK_COUNT { 2 };
		/// <summary>
		/// 傾き状態配列の、左スティックのインデックス
		/// </summary>
		static constexpr size_t LEFT_STICK_INDEX { 0 };
		/// <summary>
		/// 傾き状態配列の、右スティックのインデックス
		/// </summary>
		static constexpr size_t RIGHT_STICK_INDEX { 1 };
		/// <summary>
		/// スティックの傾き状態の配列
		/// </summary>
		std::array<StickTiltState, STICK_COUNT> stickTiltStates_;
	};

} // namespace mtgb
