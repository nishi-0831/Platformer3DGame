#pragma once

#include <array>
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
	
	class Input;
	class InputUtil final
	{
	public:
		static const bool GetKey(const KeyCode _keyCode, WindowContext _context = mtgb::WindowContext::First);
		static const bool GetKeyDown(const KeyCode _keyCode, WindowContext _context = mtgb::WindowContext::First);
		static const bool GetKeyUp(const KeyCode _keyCode, WindowContext _context = mtgb::WindowContext::First);
		static const bool GetMouse(const MouseCode _mouseCode, WindowContext _context = mtgb::WindowContext::First);
		static const bool GetMouseDown(const MouseCode _mouseCode, WindowContext _context = mtgb::WindowContext::First);
		static const bool GetMouseUp(const MouseCode _mouseCode, WindowContext _context = mtgb::WindowContext::First);
		static const bool GetGamePad(const PadCode _padButtonCode,WindowContext _context = mtgb::WindowContext::First);
		static const bool GetGamePadDown(const PadCode _padButtonCode, WindowContext _context = mtgb::WindowContext::First);
		static const bool GetGamePadUp(const PadCode _padButtonCode, WindowContext _context = mtgb::WindowContext::First);
		static const bool GetGamePad(const FlightStickCode _flightStickCode, WindowContext _context = mtgb::WindowContext::First);
		static const bool GetGamePadDown(const FlightStickCode _flightStickCode, WindowContext _context = mtgb::WindowContext::First);
		static const bool GetGamePadUp(const FlightStickCode _flightStickCode, WindowContext _context = mtgb::WindowContext::First);
		//static inline const size_t* GetActiveGamePadID() { return activeGamepadID.data(); }

		static const float GetTrigger(FlightStickAxisCode _flightStickCode, WindowContext _context = mtgb::WindowContext::First);
		static const float GetTrigger(PadAxisCode _padCode, WindowContext _context = mtgb::WindowContext::First);
		static const float GetAxis(Axis axis,WindowContext _context = mtgb::WindowContext::First);
		static const Vector2F GetAxis(WindowContext _context = mtgb::WindowContext::First);

		static const Vector2Int GetMousePosition(WindowContext _context = mtgb::WindowContext::First);
		static const Vector3 GetMouseMove(WindowContext _context = mtgb::WindowContext::First);

	private:  // Utilities
		static const size_t KEY_COUNT{ 256 };             // �L�[�̐�
		/// <summary>
		/// curr��prev��xor���擾
		/// </summary>
		/// <param name="_keyCode">�L�[�R�[�h</param>
		/// <returns>0: ������, 1: ���L��</returns>
		static inline const int KeyXOR(const KeyCode _keyCode,const std::bitset<KEY_COUNT>& _keyStateCurrent, const std::bitset<KEY_COUNT>& _keyStatePrevious)
		{
			return _keyStateCurrent[Index(_keyCode)] ^ _keyStatePrevious[Index(_keyCode)];
		}
		
		static inline const int MouseXOR(const MouseCode _mouseCode, const _DIMOUSESTATE& _mouseStateCurrent, const _DIMOUSESTATE& _mouseStatePrevious)
		{
			return _mouseStateCurrent.rgbButtons[Index(_mouseCode)] ^ _mouseStatePrevious.rgbButtons[Index(_mouseCode)];
		}

		/// <summary>
		/// �L�[�R�[�h�\���̗񋓌^���C���f�b�N�X�ɕϊ�
		/// </summary>
		/// <param name="_keyCode">�L�[�R�[�h</param>
		/// <returns>�L�[�z��̃C���f�b�N�X</returns>
		static inline const size_t Index(const KeyCode _keyCode)
		{
			return static_cast<size_t>(_keyCode);
		}

		static inline const size_t Index(const MouseCode _moudeCode)
		{
			return static_cast<size_t>(_moudeCode);
		}

		static inline const size_t Index(const PadCode _padCode)
		{
			return static_cast<size_t>(_padCode);
		}
		static inline const size_t Index(const FlightStickCode _flightStickCode)
		{
			return static_cast<size_t>(_flightStickCode);
		}

		/// <summary>
		/// ���͏�Ԃ��擾
		/// �ǂ̃E�B���h�E�ł��\��Ȃ��ꍇ��WindowContext::First�̃E�B���h�E���擾�����
		/// </summary>
		/// <param name="_context">�E�B���h�E���w��</param>
		/// <returns></returns>
		static const InputData& GetInput(WindowContext _context);
	
		static inline const int PadXOR(const PadCode _padCode, const DIJOYSTATE& _padStateCurrent, const DIJOYSTATE& _padStatePrevious)
		{
			return _padStateCurrent.rgbButtons[Index(_padCode)] ^ _padStatePrevious.rgbButtons[Index(_padCode)];
		}
		//static inline const int padXOR(const PadCode _padCode, const _XINPUT_STATE& _padStateCurrent, const _XINPUT_STATE& _padStatePrevious)
		//{
		//	WORD buttonPrev = _padStatePrevious.Gamepad.wButtons; // 0000
		//	WORD buttonCurr = _padStateCurrent.Gamepad.wButtons;  // 1000
		//	WORD padCode    = static_cast<WORD>(_padCode);        // 0x08
		//	return (buttonCurr & padCode) ^ (buttonPrev & padCode); // 1 ^ 0 = 1
		//}

		static inline bool GetGamePadImpl(size_t _index, WindowContext _context);
		static inline bool GetGamePadUpImpl(size_t _index, WindowContext _context);
		static inline bool GetGamePadDownImpl(size_t _index, WindowContext _context);
	};

	class InputData final
	{
		friend Input;
		friend InputUtil;
		friend class InputResource;
	private:  // Data
		static const size_t KEY_COUNT{ 256 };      // �L�[�̐�
		std::bitset<KEY_COUNT> keyStateCurrent_;   // �L�[�̏�Ԍ���
		std::bitset<KEY_COUNT> keyStatePrevious_;  // �L�[�̏�ԑO��
		
		_DIMOUSESTATE mouseStateCurrent_;   // �}�E�X�̏�Ԍ���
		_DIMOUSESTATE mouseStatePrevious_;  // �}�E�X�̏�ԑO��
		DIJOYSTATE joyStateCurrent_;		// �W���C�X�e�B�b�N�̏�Ԍ���
		DIJOYSTATE joyStatePrevious_;		// �W���C�X�e�B�b�N�̏�Ԍ���

		InputConfig config_;//���͂̎����̐ݒ�

		Vector2Int mousePosition_;          // �}�E�X�J�[�\���̍��W

		static const size_t GAME_PAD_COUNT{ XUSER_MAX_COUNT };            // �Q�[���p�b�h�̍ő�ڑ��\��
		std::array<_XINPUT_STATE, GAME_PAD_COUNT> gamePadStateCurrent_;   // �Q�[���p�b�h�̏�Ԍ���
		std::array<_XINPUT_STATE, GAME_PAD_COUNT> gamePadStatePrevious_;  // �Q�[���p�b�h�̏�ԑO��
		std::map<PadIDState ,int> activeGamePadID;                     // �L���ȃR���g���[����ID(0~3������)
		int gamePadID;                                                    // �R���e�L�X�g�Ɋ��蓖�Ă�ꂽ�R���g���[����ID
	
	public:
		//InputData();
		//~InputData();
	};

	
}
