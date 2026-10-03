#include "InputQuery.h"
#include "Window/WindowContext/WindowContextResourceManager.h"
#include "Window/WindowContext/InputResource.h"

#include <limits>
namespace
{
	constexpr LONG JOY_AXIS_MAX		   = 65535;
	constexpr size_t INVALID_PAD_INDEX = (std::numeric_limits<size_t>::max)();

	constexpr size_t IndexFromDualShock(PadButton _padButton)
	{
		switch (_padButton)
		{
			case PadButton ::EAST :
				return static_cast<size_t>(DualShockButtonIndex::CIRCLE);

			case PadButton::WEST :
				return static_cast<size_t>(DualShockButtonIndex::SQUARE);

			case PadButton::NORTH :
				return static_cast<size_t>(DualShockButtonIndex::TRIANGLE);

			case PadButton::SOUTH :
				return static_cast<size_t>(DualShockButtonIndex::CROSS);

			case PadButton::L_STICK :
				return static_cast<size_t>(DualShockButtonIndex::L_STICK_BUTTON);

			default :
				return INVALID_PAD_INDEX;
		}
	}
	constexpr size_t IndexFromXbox(PadButton _padButton)
	{
		switch (_padButton)
		{
			case PadButton::EAST :
				return static_cast<size_t>(XboxButtonIndex::B);

			case PadButton::WEST :
				return static_cast<size_t>(XboxButtonIndex::X);

			case PadButton::NORTH :
				return static_cast<size_t>(XboxButtonIndex::Y);

			case PadButton::SOUTH :
				return static_cast<size_t>(XboxButtonIndex::A);

			case PadButton::L_STICK :
				return static_cast<size_t>(XboxButtonIndex::L_STICK_BUTTON);

			default :
				return INVALID_PAD_INDEX;
		}
	}
} // namespace

bool mtgb::InputQuery::GetKey(KeyCode _keyCode, WindowContext _context)
{
	if (_context == WindowContext::BOTH)
	{
		return GetInput(WindowContext::FIRST).keyStateCurrent_[Index(_keyCode)] ||
			   GetInput(WindowContext::SECOND).keyStateCurrent_[Index(_keyCode)];
	}

	return GetInput(_context).keyStateCurrent_[Index(_keyCode)];
}

bool mtgb::InputQuery::GetKeyDown(KeyCode _keyCode, WindowContext _context)
{
	if (_context == WindowContext::BOTH)
	{
		const InputData& inputFirstWnd	= GetInput(WindowContext::FIRST);
		const InputData& inputSecondWnd = GetInput(WindowContext::SECOND);

		return static_cast<bool>(
				   KeyXOR(_keyCode, inputFirstWnd.keyStateCurrent_, inputFirstWnd.keyStatePrevious_) &
				   static_cast<int>(inputFirstWnd.keyStateCurrent_[Index(_keyCode)])
			   ) ||
			   static_cast<bool>(
				   KeyXOR(_keyCode, inputSecondWnd.keyStateCurrent_, inputSecondWnd.keyStatePrevious_) &
				   static_cast<int>(inputSecondWnd.keyStateCurrent_[Index(_keyCode)])
			   );
	}

	const InputData& input = GetInput(_context);
	return static_cast<bool>(
		KeyXOR(_keyCode, input.keyStateCurrent_, input.keyStatePrevious_) &
		static_cast<int>(input.keyStateCurrent_[Index(_keyCode)])
	);
}

bool mtgb::InputQuery::GetKeyUp(KeyCode _keyCode, WindowContext _context)
{
	if (_context == WindowContext::BOTH)
	{
		const InputData& inputFirstWnd	= GetInput(WindowContext::FIRST);
		const InputData& inputSecondWnd = GetInput(WindowContext::SECOND);

		return static_cast<bool>(
				   KeyXOR(_keyCode, inputFirstWnd.keyStateCurrent_, inputFirstWnd.keyStatePrevious_) &
				   static_cast<int>(inputFirstWnd.keyStatePrevious_[Index(_keyCode)])
			   ) ||
			   static_cast<bool>(
				   KeyXOR(_keyCode, inputSecondWnd.keyStateCurrent_, inputSecondWnd.keyStatePrevious_) &
				   static_cast<int>(inputSecondWnd.keyStatePrevious_[Index(_keyCode)])
			   );
	}

	const InputData& input = GetInput(_context);
	int result { KeyXOR(_keyCode, input.keyStateCurrent_, input.keyStatePrevious_) &
				 static_cast<int>(input.keyStatePrevious_[Index(_keyCode)]) };
	return static_cast<bool>(result);
}

bool mtgb::InputQuery::GetMouse(MouseButton _mouseButton, WindowContext _context)
{
	if (_context == WindowContext::BOTH)
	{
		return GetMouse(_mouseButton, WindowContext::FIRST) || GetMouse(_mouseButton, WindowContext::SECOND);
	}
	return GetInput(_context).mouseStateCurrent_.rgbButtons[Index(_mouseButton)] & 0x80;
}

bool mtgb::InputQuery::GetMouseDown(MouseButton _mouseButton, WindowContext _context)
{
	if (_context == WindowContext::BOTH)
	{
		return GetMouseDown(_mouseButton, WindowContext::FIRST) || GetMouseDown(_mouseButton, WindowContext::SECOND);
	}
	const InputData& input = GetInput(_context);
	return static_cast<bool>(
		MouseXOR(_mouseButton, input.mouseStateCurrent_, input.mouseStatePrevious_) &
		static_cast<int>(input.mouseStateCurrent_.rgbButtons[Index(_mouseButton)])
	);
}

bool mtgb::InputQuery::GetMouseUp(MouseButton _mouseButton, WindowContext _context)
{
	if (_context == WindowContext::BOTH)
	{
		return GetMouseUp(_mouseButton, WindowContext::FIRST) || GetMouseUp(_mouseButton, WindowContext::SECOND);
	}

	const InputData& input = GetInput(_context);
	return static_cast<bool>(
		MouseXOR(_mouseButton, input.mouseStateCurrent_, input.mouseStatePrevious_) &
		static_cast<int>(input.mouseStatePrevious_.rgbButtons[Index(_mouseButton)])
	);
}

bool mtgb::InputQuery::GetGamePad(PadButton _padButton, WindowContext _context)
{
	return GetGamePadImpl(Index(_padButton, _context), _context);
}

bool mtgb::InputQuery::GetGamePadDown(PadButton _padButton, WindowContext _context)
{
	return GetGamePadDownImpl(Index(_padButton, _context), _context);
}

bool mtgb::InputQuery::GetGamePadUp(PadButton _padButton, WindowContext _context)
{
	return GetGamePadUpImpl(Index(_padButton, _context), _context);
}

bool mtgb::InputQuery::GetGamePad(FlightStickButton _flightStickButton, WindowContext _context)
{
	return GetGamePadImpl(Index(_flightStickButton), _context);
}

bool mtgb::InputQuery::GetGamePadDown(FlightStickButton _flightStickButton, WindowContext _context)
{
	return GetGamePadDownImpl(Index(_flightStickButton), _context);
}

bool mtgb::InputQuery::GetGamePadUp(FlightStickButton _flightStickButton, WindowContext _context)
{
	return GetGamePadUpImpl(Index(_flightStickButton), _context);
}

size_t mtgb::InputQuery::Index(PadButton _padCode, WindowContext _context)
{
	const InputData* inputData = Game::System<WindowContextResourceManager>().Get<InputResource>(_context).GetInput();
	GamePadType controllerType = inputData->gamePadType_;
	switch (controllerType)
	{
		case mtgb::GamePadType::DUAL_SHOCK :
			return IndexFromDualShock(_padCode);
		case mtgb::GamePadType::XBOX :
			return IndexFromXbox(_padCode);
		case mtgb::GamePadType::UNKNOWN :
		default :
			return INVALID_PAD_INDEX;
	}
}

const mtgb::InputData& mtgb::InputQuery::GetInput(WindowContext _context)
{
	if (_context == WindowContext::BOTH)
	{
		return *(Game::System<WindowContextResourceManager>().Get<InputResource>(WindowContext::FIRST).GetInput());
	}
	return *(Game::System<WindowContextResourceManager>().Get<InputResource>(_context).GetInput());
}

bool mtgb::InputQuery::GetGamePadImpl(size_t _index, WindowContext _context)
{
	if (_index == INVALID_PAD_INDEX)
	{
		return false;
	}
	if (_context == WindowContext::BOTH)
	{
		return GetGamePadImpl(_index, WindowContext::FIRST) || GetGamePadImpl(_index, WindowContext::SECOND);
	}
	return GetInput(_context).joyStateCurrent_.rgbButtons[_index];
}

bool mtgb::InputQuery::GetGamePadUpImpl(size_t _index, WindowContext _context)
{
	if (_index == INVALID_PAD_INDEX)
	{
		return false;
	}
	if (_context == WindowContext::BOTH)
	{
		return GetGamePadUpImpl(_index, WindowContext::FIRST) || GetGamePadUpImpl(_index, WindowContext::SECOND);
	}

	const InputData& input = GetInput(_context);
	int XOR				   = input.joyStateCurrent_.rgbButtons[_index] ^ input.joyStatePrevious_.rgbButtons[_index];
	int prev			   = input.joyStatePrevious_.rgbButtons[_index];
	return static_cast<bool>(XOR & prev);
}

bool mtgb::InputQuery::GetGamePadDownImpl(size_t _index, WindowContext _context)
{
	if (_index == INVALID_PAD_INDEX)
	{
		return false;
	}
	if (_context == WindowContext::BOTH)
	{
		return GetGamePadDownImpl(_index, WindowContext::FIRST) || GetGamePadDownImpl(_index, WindowContext::SECOND);
	}

	const InputData& input = GetInput(_context);
	int XOR				   = input.joyStateCurrent_.rgbButtons[_index] ^ input.joyStatePrevious_.rgbButtons[_index];
	int curr			   = input.joyStateCurrent_.rgbButtons[_index];
	return static_cast<bool>(XOR & curr);
}

float mtgb::InputQuery::GetAxis(Axis _axis, StickType _stickType, WindowContext _context)
{
	const InputData& input = GetInput(_context);
	float value			   = 0.0f;
	switch (_axis)
	{
		case Axis::X :
			if (_stickType == StickType::LEFT)
			{
				value = static_cast<float>(input.joyStateCurrent_.lX);
			}
			else
			{
				if (input.gamePadType_ == GamePadType::DUAL_SHOCK)
				{
					value = static_cast<float>(input.joyStateCurrent_.lZ);
				}
				else if (input.gamePadType_ == GamePadType::XBOX)
				{
					value = static_cast<float>(input.joyStateCurrent_.lRx);
				}
				else
				{
					value = static_cast<float>(input.joyStateCurrent_.lRx);
				}
			}
			value /= input.config_.xRange;
			break;

		case Axis::Y :
			if (_stickType == StickType::LEFT)
			{
				value = static_cast<float>(input.joyStateCurrent_.lY);
			}
			else
			{
				if (input.gamePadType_ == GamePadType::DUAL_SHOCK)
				{
					value = static_cast<float>(input.joyStateCurrent_.lRz);
				}
				else if (input.gamePadType_ == GamePadType::XBOX)
				{
					value = -static_cast<float>(input.joyStateCurrent_.lRy);
				}
				else
				{
					value = static_cast<float>(input.joyStateCurrent_.lRy);
				}
			}
			value /= input.config_.yRange;
			break;
		default :
			return 0.0f;
	}
	return input.config_.ApplyDeadZone(value);
}

mtgb::Vector2F mtgb::InputQuery::GetAxis(StickType _stickType, WindowContext _context)
{
	return Vector2F { GetAxis(Axis::X, _stickType, _context), GetAxis(Axis::Y, _stickType, _context) };
}

bool mtgb::InputQuery::GetStickDown(Axis _axis, StickType _stickType, StickDirection _stickDir, WindowContext _context)
{
	float axis				   = GetAxis(_axis, _stickType, _context);
	const InputData& inputData = InputQuery::GetInput(_context);

	size_t idx = static_cast<size_t>(_stickType);
	bool prev  = inputData.stickTiltStates_[idx].isFullyTiltedPrev_;
	bool curr  = inputData.stickTiltStates_[idx].isFullyTiltedCurr_;

	if (prev == curr || curr == false)
		return false;

	if (_stickDir == StickDirection::Positive)
	{
		return (axis == 1.0f);
	}
	else
	{
		return (axis == -1.0f);
	}
}

mtgb::Vector2Int mtgb::InputQuery::GetMousePosition(WindowContext _context)
{
	return InputQuery::GetInput(_context).mousePosition_;
}

mtgb::Vector3 mtgb::InputQuery::GetMouseMove(WindowContext _context)
{
	return mtgb::Vector3 {
		static_cast<float>(InputQuery::GetInput(_context).mouseStateCurrent_.lX),
		static_cast<float>(InputQuery::GetInput(_context).mouseStateCurrent_.lY),
		static_cast<float>(InputQuery::GetInput(_context).mouseStateCurrent_.lZ),
	};
}

mtgb::Vector3 mtgb::InputQuery::GetMouseAxis(WindowContext _context)
{
	const InputData& input = GetInput(_context);
	float value			   = 0.0f;
	return mtgb::Vector3 { input.config_.NormalizeMouseMovement(input.mouseStateCurrent_.lX),
						   input.config_.NormalizeMouseMovement(input.mouseStateCurrent_.lY),
						   input.config_.NormalizeMouseMovement(input.mouseStateCurrent_.lZ) };
}