#pragma once
#include <bitset>

#include "Math/Vector3.h"
#include "Math/Vector2Int.h"
#include "Math/Vector2F.h"
#include "InputKeyCode.h"
#include "InputMouseButton.h"
#include "InputPadButton.h"
#include "Axis.h"
#include "Window/WindowContext/WindowContext.h"
#include "InputData.h"
namespace mtgb
{
	enum class StickDirection
	{
		Positive, // スティックの正方向(右 / 上)
		Negative  // スティックの負方向(左 / 下)
	};

	class InputQuery
	{
	  public:
		static bool GetKey(KeyCode _keyCode, WindowContext _context = mtgb::WindowContext::FIRST);
		static bool GetKeyDown(KeyCode _keyCode, WindowContext _context = mtgb::WindowContext::FIRST);
		static bool GetKeyUp(KeyCode _keyCode, WindowContext _context = mtgb::WindowContext::FIRST);
		static bool GetMouse(MouseButton _mouseButton, WindowContext _context = mtgb::WindowContext::FIRST);
		static bool GetMouseDown(MouseButton _mouseButton, WindowContext _context = mtgb::WindowContext::FIRST);
		static bool GetMouseUp(MouseButton _mouseButton, WindowContext _context = mtgb::WindowContext::FIRST);
		static bool GetGamePad(PadButton _padButton, WindowContext _context = mtgb::WindowContext::FIRST);
		static bool GetGamePadDown(PadButton _padButton, WindowContext _context = mtgb::WindowContext::FIRST);
		static bool GetGamePadUp(PadButton _padButton, WindowContext _context = mtgb::WindowContext::FIRST);
		static bool GetGamePad(
			FlightStickButton _flightStickButton,
			WindowContext _context = mtgb::WindowContext::FIRST
		);
		static bool GetGamePadDown(
			FlightStickButton _flightStickButton,
			WindowContext _context = mtgb::WindowContext::FIRST
		);
		static bool GetGamePadUp(
			FlightStickButton _flightStickButton,
			WindowContext _context = mtgb::WindowContext::FIRST
		);

		static float GetAxis(Axis _axis, StickType _stickType, WindowContext _context = mtgb::WindowContext::FIRST);
		static Vector2F GetAxis(StickType _stickType, WindowContext _context = mtgb::WindowContext::FIRST);
		/// <summary>
		/// スティックが傾けられたか
		/// </summary>
		/// <param name="_axis">傾けた軸(X,Y)</param>
		/// <param name="_stickType">スティックの種類(左スティックか右スティックか)</param>
		/// <param name="_stickDir">傾けた方向(正方向、負方向)</param>
		/// <param name="_context">ウィンドウの識別子</param>
		/// <returns></returns>
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
			MouseButton _mouseButton,
			const _DIMOUSESTATE& _mouseStateCurrent,
			const _DIMOUSESTATE& _mouseStatePrevious
		)
		{
			return _mouseStateCurrent.rgbButtons[Index(_mouseButton)] ^
				   _mouseStatePrevious.rgbButtons[Index(_mouseButton)];
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

		static inline size_t Index(MouseButton _moudeCode)
		{
			return static_cast<size_t>(_moudeCode);
		}

		static inline size_t Index(PadButton _padCode)
		{
			return static_cast<size_t>(_padCode);
		}
		static inline size_t Index(FlightStickButton _flightStickButton)
		{
			return static_cast<size_t>(_flightStickButton);
		}

		static size_t Index(PadButton _padCode, WindowContext _context);

		/// <summary>
		/// 入力状態を取得
		/// どのウィンドウでも構わない場合はWindowContext::Firstのウィンドウが取得される
		/// </summary>
		/// <param name="_context">ウィンドウを指定</param>
		/// <returns></returns>
		static const InputData& GetInput(WindowContext _context);

		static inline int PadXOR(
			PadButton _padCode,
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
} // namespace mtgb