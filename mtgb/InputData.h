#pragma once

#include <array>
#include <unordered_map>
#include "IncludingInput.h"
#include <bitset>

#include "Vector3.h"
#include "Vector2Int.h"
#include "Vector2F.h"
#include "InputKeyCode.h"
#include "InputMouseCode.h"
#include "InputPadCode.h"

#include "cmtgb.h"
#include "WindowContext.h"
#include "InputConfig.h"
#include "Input.h"
#include "Axis.h"

typedef struct HWND__* HWND;
namespace mtgb
{
	enum class StickDirection
	{
		Positive, // スティックの正方向(右 / 上)
		Negative  // スティックの負方向(左 / 下)
	};
	class Input;
	class InputUtil final
	{
	  public:
		static bool GetKey(KeyCode _keyCode, WindowContext _context = mtgb::WindowContext::FIRST);
		static bool GetKeyDown(KeyCode _keyCode, WindowContext _context = mtgb::WindowContext::FIRST);
		static bool GetKeyUp(KeyCode _keyCode, WindowContext _context = mtgb::WindowContext::FIRST);
		static bool GetMouse(MouseCode _mouseCode, WindowContext _context = mtgb::WindowContext::FIRST);
		static bool GetMouseDown(MouseCode _mouseCode, WindowContext _context = mtgb::WindowContext::FIRST);
		static bool GetMouseUp(MouseCode _mouseCode, WindowContext _context = mtgb::WindowContext::FIRST);
		static bool GetGamePad(PadCode _padButtonCode, WindowContext _context = mtgb::WindowContext::FIRST);
		static bool GetGamePadDown(PadCode _padButtonCode, WindowContext _context = mtgb::WindowContext::FIRST);
		static bool GetGamePadUp(PadCode _padButtonCode, WindowContext _context = mtgb::WindowContext::FIRST);
		static bool GetGamePad(FlightStickCode _flightStickCode, WindowContext _context = mtgb::WindowContext::FIRST);
		static bool GetGamePadDown(
			FlightStickCode _flightStickCode,
			WindowContext _context = mtgb::WindowContext::FIRST
		);
		static bool GetGamePadUp(FlightStickCode _flightStickCode, WindowContext _context = mtgb::WindowContext::FIRST);

		static float GetTrigger(
			FlightStickAxisCode _flightStickCode,
			WindowContext _context = mtgb::WindowContext::FIRST
		);
		static float GetTrigger(PadAxisCode _padCode, WindowContext _context = mtgb::WindowContext::FIRST);
		static float GetAxis(Axis _axis, StickType _stickType, WindowContext _context = mtgb::WindowContext::FIRST);
		static Vector2F GetAxis(StickType _stickType, WindowContext _context = mtgb::WindowContext::FIRST);
		static bool GetStickDown(
			Axis _axis,
			StickType _stickType,
			StickDirection _stickDir,
			WindowContext _context = mtgb::WindowContext::FIRST
		);

		static Vector2Int GetMousePosition(WindowContext _context = mtgb::WindowContext::FIRST);
		static Vector3 GetMouseMove(WindowContext _context = mtgb::WindowContext::FIRST);
		static Vector3 GetMouseAxis(WindowContext _context = mtgb::WindowContext::FIRST);

	  private:								   // Utilities
		static const size_t KEY_COUNT { 256 }; // キーの数
		/// <summary>
		/// currとprevのxorを取得
		/// </summary>
		/// <param name="_keyCode">キーコード</param>
		/// <returns>0: 差無し, 1: 差有り</returns>
		static inline int KeyXOR(
			KeyCode _keyCode,
			const std::bitset<KEY_COUNT>& _keyStateCurrent,
			const std::bitset<KEY_COUNT>& _keyStatePrevious
		)
		{
			return _keyStateCurrent[Index(_keyCode)] ^ _keyStatePrevious[Index(_keyCode)];
		}

		static inline int MouseXOR(
			MouseCode _mouseCode,
			const _DIMOUSESTATE& _mouseStateCurrent,
			const _DIMOUSESTATE& _mouseStatePrevious
		)
		{
			return _mouseStateCurrent.rgbButtons[Index(_mouseCode)] ^ _mouseStatePrevious.rgbButtons[Index(_mouseCode)];
		}

		/// <summary>
		/// キーコード構造体列挙型をインデックスに変換
		/// </summary>
		/// <param name="_keyCode">キーコード</param>
		/// <returns>キー配列のインデックス</returns>
		static inline size_t Index(KeyCode _keyCode)
		{
			return static_cast<size_t>(_keyCode);
		}

		static inline size_t Index(MouseCode _moudeCode)
		{
			return static_cast<size_t>(_moudeCode);
		}

		static inline size_t Index(PadCode _padCode)
		{
			return static_cast<size_t>(_padCode);
		}
		static inline size_t Index(FlightStickCode _flightStickCode)
		{
			return static_cast<size_t>(_flightStickCode);
		}

		/// <summary>
		/// 入力状態を取得
		/// どのウィンドウでも構わない場合はWindowContext::Firstのウィンドウが取得される
		/// </summary>
		/// <param name="_context">ウィンドウを指定</param>
		/// <returns></returns>
		static const InputData& GetInput(WindowContext _context);

		static inline int PadXOR(
			PadCode _padCode,
			const DIJOYSTATE& _padStateCurrent,
			const DIJOYSTATE& _padStatePrevious
		)
		{
			return _padStateCurrent.rgbButtons[Index(_padCode)] ^ _padStatePrevious.rgbButtons[Index(_padCode)];
		}

		static bool GetGamePadImpl(size_t _index, WindowContext _context);
		static bool GetGamePadUpImpl(size_t _index, WindowContext _context);
		static bool GetGamePadDownImpl(size_t _index, WindowContext _context);
	};

	class InputData final
	{
		friend Input;
		friend InputUtil;
		friend class InputResource;

	  private:
		void Initialize();
		static inline constexpr size_t KEY_COUNT { 256 }; // キーの数
		std::bitset<KEY_COUNT> keyStateCurrent_;		  // キーの状態現在
		std::bitset<KEY_COUNT> keyStatePrevious_;		  // キーの状態前回

		_DIMOUSESTATE mouseStateCurrent_;  // マウスの状態現在
		_DIMOUSESTATE mouseStatePrevious_; // マウスの状態前回
		DIJOYSTATE joyStateCurrent_;	   // ジョイスティックの状態現在
		DIJOYSTATE joyStatePrevious_;	   // ジョイスティックの状態現在
		ControllerType controllerType_;	   // コントローラーの種類
		InputConfig config_;			   // 入力の取り方の設定

		Vector2Int mousePosition_; // マウスカーソルの座標

		struct StickTiltState
		{
			bool isFullyTiltedPrev_; // スティックが完全に傾けられているか 前回
			bool isFullyTiltedCurr_; // スティックが完全に傾けられているか 現在
		};
		std::array<StickTiltState, 2> stickTiltStates_;
	};

} // namespace mtgb
