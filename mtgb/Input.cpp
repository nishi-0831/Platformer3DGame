#include "Input.h"
#include "IncludingWindows.h"
#include "IncludingInput.h"
#include "InputData.h"
#include "ReleaseUtility.h"
#include "MTAssert.h"
#include "MainWindow.h"
#include "DoubleWindow.h"
#include "InputResource.h"
#include <algorithm>
#include "Game.h"
#include "ISystem.h"
#include "Debug.h"
#include "../ImGui/imgui.h"
#include "Timer.h"

namespace
{
	static const size_t KEY_BUFFER_SIZE{ 256 };
	LONG min = -1000;
	LONG max = 1000;
	LONG xMin{ min },
		 xMax{ max },
		 yMin{ min },
		 yMax{ max };
	float acquireInterval = 10.0f;
}

using namespace mtgb;

void mtgb::Input::AcquireJoystick(ComPtr<IDirectInputDevice8> _pJoystickDevice)
{
	HRESULT hResult{};
	hResult = _pJoystickDevice->Acquire();
	joystickContext_[currJoystickGuid_].lastResult = hResult;
	switch (hResult)
	{
	case DI_OK:  //�擾�ł���
	case S_FALSE://���̃A�v���������擾���Ă���
		break;
	case DIERR_OTHERAPPHASPRIO://���̃A�v�����D�挠�������Ă���
		return;
	case DIERR_INVALIDPARAM:
	case DIERR_NOTINITIALIZED:
		massert(SUCCEEDED(hResult)
			&& "�W���C�X�e�B�b�N����̋��擾�̍ۂɃG���[���N����܂��� @Input::Update");
		return;
	default:
		break;
	}
}

GUID mtgb::Input::GetDeviceGuid(ComPtr<IDirectInputDevice8> _pInputDevice)
{
	DIDEVICEINSTANCE deviceInstance = {};
	deviceInstance.dwSize = sizeof(DIDEVICEINSTANCE);
	HRESULT hResult = _pInputDevice->GetDeviceInfo(&deviceInstance);
	massert(SUCCEEDED(hResult)
		&& "�f�o�C�X�̏��̎擾�Ɏ��s���܂��� @Input::Update");
	return deviceInstance.guidInstance;
}

bool operator<(const GUID& lhs, const GUID& rhs)
{
	return std::memcmp(&lhs, &rhs, sizeof(GUID)) < 0;
}


mtgb::Input::Input() :
	pInputData_  { nullptr },
	pDirectInput_{ nullptr },
	pKeyDevice_  { nullptr },
	pMouseDevice_{ nullptr }
{
}

mtgb::Input::~Input()
{
	Release();
	
}

void mtgb::Input::Initialize()
{
	HRESULT hResult{};

	// DirectInput8�̃f�o�C�X�쐬
	hResult = DirectInput8Create(
		GetModuleHandle(nullptr),
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		reinterpret_cast<void**>(pDirectInput_.GetAddressOf()),
		nullptr);

	massert(SUCCEEDED(hResult)  // DirectInput8�̃f�o�C�X�쐬�ɐ���
		&& "DirectInput8�̃f�o�C�X�쐬�Ɏ��s @Input::Initialize");

}

void mtgb::Input::Update()
{
	static HRESULT hResult{};

#pragma region �L�[�{�[�h
	UpdateKeyDevice();
#pragma endregion

#pragma region �}�E�X
	UpdateMouseDevice();
#pragma endregion

#pragma region �W���C�X�e�B�b�N
	
	/*if (pJoystickDevice_ == nullptr)
	{
		return;
	}*/
	UpdateJoystickDevice();
	
#pragma endregion
	if (InputUtil::GetKeyDown(KeyCode::P))
	{
		EnumJoystick();
	}

#pragma region �Q�[���p�b�h
	//UpdateGamePadDevice();
#pragma endregion
}

void mtgb::Input::UpdateKeyDevice()
{
	static HRESULT hResult{};
	// �L�[�{�[�h����̋��Q�b�g
	hResult = pKeyDevice_->Acquire();

	//massert(SUCCEEDED(hResult)  // �L�[�{�[�h����̋��擾�ɐ���
	//	&& "�L�[�{�[�h����̋��擾�Ɏ��s @Input::Update");

	if (FAILED(hResult))
	{
		return;  // �L�[�{�[�h����̋��擾�Ɏ��s�����Ȃ��A
	}

	static BYTE keyBuffer[KEY_BUFFER_SIZE]{};  // �L�[��Ԏ擾�p�o�b�t�@

	pInputData_->keyStatePrevious_ = pInputData_->keyStateCurrent_;
	pKeyDevice_->GetDeviceState(KEY_BUFFER_SIZE, keyBuffer);

	// TODO: for�ŉ񂷂̂̓R�X�p�悭�Ȃ�
	for (int i = 0; i < KEY_BUFFER_SIZE; i++)
	{
		pInputData_->keyStateCurrent_[i] = keyBuffer[i];
	}
}

void mtgb::Input::UpdateMouseDevice()
{
	static HRESULT hResult{};

	// �}�E�X����̋����Q�b�g
	hResult = pMouseDevice_->Acquire();

	if (FAILED(hResult))
	{
		return;  // �}�E�X����̋��擾�Ɏ��s�����Ȃ��A
	}

	massert(SUCCEEDED(hResult)  // �}�E�X����̋��擾�ɐ���
		&& "�}�E�X����̋��擾�Ɏ��s @Input::Update");

	memcpy(
		&pInputData_->mouseStatePrevious_,
		&pInputData_->mouseStateCurrent_,
		sizeof(DIMOUSESTATE));


	hResult = pMouseDevice_->GetDeviceState(
		sizeof(DIMOUSESTATE),
		&pInputData_->mouseStateCurrent_);

	massert(SUCCEEDED(hResult)  // �}�E�X����̎擾�ɐ���
		&& "�}�E�X����̎擾�Ɏ��s @Input::Update");

#pragma endregion
}

void mtgb::Input::UpdateJoystickDevice()
{
	static HRESULT hResult{};
	
	if (joystickContext_.empty()) return;
	if (currJoystickGuid_ == GUID_NULL) return;

	memcpy(
		&pInputData_->joyStatePrevious_,
		&pInputData_->joyStateCurrent_,
		sizeof(DIJOYSTATE));

	hResult = joystickContext_[currJoystickGuid_].device->GetDeviceState(sizeof(DIJOYSTATE), &pInputData_->joyStateCurrent_);
	joystickContext_[currJoystickGuid_].lastResult = hResult;
	switch (hResult)
	{
	case DI_OK:
		// LOGF("OK\n");
		break;
	case DIERR_INPUTLOST:// ���̓��X�g�A�ꎞ�I�ȃA�N�Z�X�s��
		AcquireJoystick(joystickContext_[currJoystickGuid_].device);
		return;
	case DIERR_NOTACQUIRED:// ���擾
		AcquireJoystick(joystickContext_[currJoystickGuid_].device);
		return;
	default: // ���炩�̎��s
	{
		// �f�o�C�X�����蓖�čς݃��X�g���珜�O
		UnregisterJoystickGuid(GetDeviceGuid(joystickContext_[currJoystickGuid_].device));
		return;
	}
	/* massert(false
		&& "�f�o�C�X�̏�Ԃ̎擾�̍ۂɃG���[���N����܂��� @Input::Update");*/
	}
}

void mtgb::Input::UpdateGamePadDevice()
{
	// TODO: �֐�������I

	// �A�N�e�B�u�ȃR���g���[�����Ȃ���΁A���^�[���B
	{
		bool IS_GAMEPAD_DETECTED = std::any_of(pInputData_->activeGamePadID.begin(),
											   pInputData_->activeGamePadID.end(),
											   [](std::pair<const PadIDState, int> _id) { return _id.second != -1; });
		if (not(IS_GAMEPAD_DETECTED))
		{
			CheckValidPadID();
		}
	}

	// �R���g���[���̊��蓖��
	// ������ID�ł���Ώ�������
	// ���蓖�Ă�ID�̃L�[��ASSIGNED�ɂ���
	// 

	for (int i = 0; i < XUSER_MAX_COUNT; i++)
	{
		// Previous��Current�̏�Ԃ��R�s�[
		memcpy(
			&pInputData_->gamePadStatePrevious_[i],
			&pInputData_->gamePadStateCurrent_[i],
			sizeof(_XINPUT_STATE));

		// ���݂̃R���g���[���[�̏�Ԃ��擾
		XInputGetState(i, &pInputData_->gamePadStateCurrent_[i]); // �����ŃG���[����!
	}
}

void mtgb::Input::Release()
{
	pMouseDevice_.Reset();
	pKeyDevice_.Reset();
	pDirectInput_.Reset();
}

void mtgb::Input::UpdateMousePositionData(
	const int32_t _x,
	const int32_t _y)
{
	if (pInputData_)
	{
		pInputData_->mousePosition_.x = _x;
		pInputData_->mousePosition_.y = _y;
	}
}

void mtgb::Input::CreateKeyDevice(HWND _hWnd, LPDIRECTINPUTDEVICE8* _ppKeyDevice)
{
	HRESULT hResult{};

	hResult = pDirectInput_->CreateDevice(GUID_SysKeyboard, _ppKeyDevice, nullptr);
	massert(SUCCEEDED(hResult)  // �L�[�{�[�h�f�o�C�X�̍쐬�ɐ���
		&& "�L�[�{�[�h�f�o�C�X�̍쐬�Ɏ��s @Input::CreateKeyDevice");

	// �L�[�{�[�h�p�Ƀt�H�[�}�b�g
	hResult = (*_ppKeyDevice)->SetDataFormat(&c_dfDIKeyboard);

	massert(SUCCEEDED(hResult)  // �L�[�{�[�h�t�H�[�}�b�g�ɐ���
		&& "�L�[�{�[�h�t�H�[�}�b�g�Ɏ��s @Input::CreateDevice");

	// �L�[�{�[�h�̃A�v���ԋ��L���x����ݒ�
	//  REF: https://learn.microsoft.com/ja-jp/previous-versions/windows/desktop/ee417921(v=vs.85)
	//hResult = (*_ppKeyDevice)->SetCooperativeLevel(_hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	//��A�N�e�B�u�ȃA�v�������͂��󂯕t����
	hResult = (*_ppKeyDevice)->SetCooperativeLevel(_hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);

	massert(SUCCEEDED(hResult)  // �L�[�{�[�h�A�v���ԋ��L���x���ݒ�ɐ���
		&& "�L�[�{�[�h�A�v���ԋ��L���x���ݒ�Ɏ��s @Input::CreateDevice");
}

void mtgb::Input::CreateMouseDevice(HWND _hWnd, LPDIRECTINPUTDEVICE8* _ppMouseDevice)
{
	HRESULT hResult{};

	hResult = pDirectInput_->CreateDevice(GUID_SysMouse, _ppMouseDevice, nullptr);
	massert(SUCCEEDED(hResult)  // �L�[�{�[�h�f�o�C�X�̍쐬�ɐ���
		&& "�}�E�X�f�o�C�X�̍쐬�Ɏ��s @Input::CreateMouseDevice");

	// �}�E�X�p�Ƀt�H�[�}�b�g
	hResult = (*_ppMouseDevice)->SetDataFormat(&c_dfDIMouse);

	massert(SUCCEEDED(hResult)  // �}�E�X�t�H�[�}�b�g�ɐ���
		&& "�}�E�X�t�H�[�}�b�g�Ɏ��s @Input::CreateMouseDevice");

	// �}�E�X�̃A�v���ԋ��L���x���̐ݒ�
	//hResult = (*_ppMouseDevice)->SetCooperativeLevel(_hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	//��A�N�e�B�u�ȃA�v�������͂��󂯕t����
	hResult = (*_ppMouseDevice)->SetCooperativeLevel(_hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);

	massert(SUCCEEDED(hResult)  // �}�E�X�A�v���ԋ��L���x���ݒ�ɐ���
		&& "�}�E�X�A�v���ԋ��L���x���ݒ�Ɏ��s @Input::CreateMouseDevice");
}

void mtgb::Input::ChangeKeyDevice(ComPtr<IDirectInputDevice8> _pKeyDevice)
{
	pKeyDevice_ = _pKeyDevice;
}

void mtgb::Input::SetJoystickGuid(GUID _guid)
{
	/*massert(assignedJoystickGuids_.contains(_guid)
		&& "������GUID���n����܂��� @Input::SetJoystickGuid");*/
	currJoystickGuid_ = _guid;
}

void mtgb::Input::ChangeMouseDevice(ComPtr<IDirectInputDevice8> _pMouseDevice)
{
	pMouseDevice_ = _pMouseDevice;
}

void mtgb::Input::ChangeInputData(InputData* _pInputData)
{
	pInputData_ = _pInputData;
}


void mtgb::Input::CheckValidPadID()
{
	for (int i = 0; i < XUSER_MAX_COUNT; i++)
	{
		DWORD result = XInputGetState(i, &pInputData_->gamePadStateCurrent_[i]);

		if (result == ERROR_SUCCESS)
		{
			pInputData_->activeGamePadID.insert(std::make_pair(PadIDState::UNASSIGNED, i));
		}
		else
		{
			pInputData_->activeGamePadID.insert(std::make_pair(PadIDState::INVALID, i));
		}
	}
}

void mtgb::Input::ChangeJoystickDevice(ComPtr<IDirectInputDevice8> _pJoystickDevice)
{
	pJoystickDevice_ = _pJoystickDevice;
}

/// <summary>
/// �W���C�X�e�B�b�N���ڑ�����Ă���ꍇ�A�f�o�C�X�Ɋ��蓖�Ă�
/// </summary>
/// <param name="lpddi">�f�o�C�X�̏������C���X�^���X</param>
/// <param name="pvRef">EnumDevices�œn�����l</param>
/// <returns></returns>
BOOL CALLBACK EnumJoysticksCallback(const LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
	auto& input = Game::System<Input>();

	// ���蓖�ė\�񂪂Ȃ�������f�o�C�X���쐬���Ȃ�
	if (input.IsNotSubscribed())
	{
		return DIENUM_STOP;
	}
	DeviceType devType = Input::GetDeviceType(*lpddi);

	int reservationIndex = input.FindReservationIndexForDevice(devType);

	if(reservationIndex < 0)
	{
		// �f�o�C�X��v�����Ă���\��͂Ȃ�
		return DIENUM_CONTINUE;
	}
	LPDIRECTINPUT8 pDirectInput = reinterpret_cast<LPDIRECTINPUT8>(pvRef);
	ComPtr<IDirectInputDevice8> pDevice = nullptr;

	// ���蓖�ė\�񂪂���A�������Ȃ̂Ńf�o�C�X�쐬
	HRESULT hResult = pDirectInput->CreateDevice(lpddi->guidInstance, pDevice.GetAddressOf(), nullptr);
	massert(SUCCEEDED(hResult)
		&& "�W���C�X�e�B�b�N�̃f�o�C�X�̍쐬�Ɏ��s @EnumJoysticksCallback");
	
	if (Game::System<Input>().RegisterJoystickGuid(lpddi->guidInstance) == false)
	{
		//���Ɋ��蓖�čςׁ݂̈A���̃f�o�C�X�̗񋓂Ɉڂ�
		pDevice.Reset();
		return DIENUM_CONTINUE;
	}

	input.AssignJoystickToReservation(pDevice, static_cast<size_t>(reservationIndex), lpddi->guidInstance);
	LOGIMGUI_CAT("Input", "Assigned reservationIndex=%d", reservationIndex);

	// �\�񂪂܂��c���Ă���Ȃ瑱�s
	return input.IsNotSubscribed() ? DIENUM_STOP : DIENUM_CONTINUE;
}

void mtgb::Input::EnumJoystick()
{
	// ���蓖�ė\�񂪂Ȃ�������f�o�C�X���쐬���Ȃ�
	if (Game::System<Input>().IsNotSubscribed())
	{
		return;
	}
	pDirectInput_->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, pDirectInput_.Get(), DIEDFL_ATTACHEDONLY);
}

void mtgb::Input::RequestJoystickDevice(const JoystickReservation& _reservation)
{
	requestedJoystickDevices_.push_back(_reservation);
	StartEnumTimer();
}

void mtgb::Input::RequestJoystickDevice(JoystickReservation&& _reservation)
{
	requestedJoystickDevices_.push_back(std::move(_reservation));
	StartEnumTimer();
}

void mtgb::Input::AssignJoystickToReservation(ComPtr<IDirectInputDevice8> _pJoystickDevice, size_t _reservationIndex, GUID _guid)
{
	if (_reservationIndex >= requestedJoystickDevices_.size())
		return;

	// �\������[�u
	JoystickReservation reservation = std::move(requestedJoystickDevices_[_reservationIndex]);
	requestedJoystickDevices_.erase(requestedJoystickDevices_.begin() + _reservationIndex);

	// �������x�����ݒ�
	_pJoystickDevice->SetCooperativeLevel(reservation.hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
	//_pJoystickDevice->SetCooperativeLevel(reservation.hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	_pJoystickDevice->SetDataFormat(&c_dfDIJoystick);
	SetProperty(_pJoystickDevice, reservation.config);

	//�f�o�C�X����JoystickContext�\�z
	GUID guid = GetDeviceGuid(_pJoystickDevice);
	auto [itr, inserted] = joystickContext_.emplace(guid,_pJoystickDevice);

	if (reservation.onAssign)
		reservation.onAssign(itr->second.device, guid);

	//SetAcquireInterval(guid, itr->second.device);

}

void mtgb::Input::UnregisterJoystickGuid(GUID _guid)
{
	Timer::Remove(joystickContext_[_guid].timerHandle);
	joystickContext_.erase(_guid);
}

bool mtgb::Input::RegisterJoystickGuid(GUID _guid)
{
	return assignedJoystickGuids_.insert(_guid).second;
}

void mtgb::Input::SetAcquireInterval(GUID _guid, ComPtr<IDirectInputDevice8> _device)
{
	TimerHandle hTimer = Timer::AddInterval(acquireInterval, [&]() {AcquireJoystick(_device); });
	joystickContext_[_guid].timerHandle = hTimer;
}

bool mtgb::Input::IsNotSubscribed()
{
	return requestedJoystickDevices_.empty();
}

std::string mtgb::Input::GetDeviceName(ComPtr<IDirectInputDevice8> _pInputDevice)
{
	DIDEVICEINSTANCE deviceInstance = {};
	deviceInstance.dwSize = sizeof(DIDEVICEINSTANCE);
	HRESULT hResult = _pInputDevice->GetDeviceInfo(&deviceInstance);
	massert(SUCCEEDED(hResult)
		&& "�f�o�C�X�̏��̎擾�Ɏ��s���܂����@@Input::GetDeviceName");

	return std::string(deviceInstance.tszInstanceName);
}

std::string mtgb::Input::GetDeviceName(GUID _guid)
{
	if (joystickContext_.contains(_guid))
	{
		return GetDeviceName(joystickContext_[_guid].device);
	}
	return "None";
}

std::string mtgb::Input::GetDeviceProductName(ComPtr<IDirectInputDevice8> _pInputDevice)
{
	DIDEVICEINSTANCE deviceInstance = {};
	deviceInstance.dwSize = sizeof(DIDEVICEINSTANCE);
	HRESULT hResult = _pInputDevice->GetDeviceInfo(&deviceInstance);
	massert(SUCCEEDED(hResult)
		&& "�f�o�C�X�̏��̎擾�Ɏ��s���܂����@@Input::GetDeviceName");

	return std::string(deviceInstance.tszProductName);
}

std::string mtgb::Input::GetDeviceProductName(GUID _guid)
{
	if (joystickContext_.contains(_guid))
	{
		return GetDeviceProductName(joystickContext_[_guid].device);
	}
	return "None";
}

std::string mtgb::Input::ConvertHResultToMessage(HRESULT hr) const
{
	switch (hr)
	{
	case DI_OK: return "�擾";
	case S_FALSE:return "���A�v���Ƌ��L";
	case DIERR_INPUTLOST:return "�ؒf";
	case DIERR_NOTACQUIRED:return "�f�o�C�X���擾";
	case DIERR_OTHERAPPHASPRIO: return "�����D�挠������";
	default:return"�s���ȃG���[";
	}
}

DeviceType mtgb::Input::GetDeviceType(ComPtr<IDirectInputDevice8> _pInputDevice)
{
	DIDEVICEINSTANCE deviceInstance = {};
	deviceInstance.dwSize = sizeof(DIDEVICEINSTANCE);
	HRESULT hResult = _pInputDevice->GetDeviceInfo(&deviceInstance);

	massert(SUCCEEDED(hResult)
		&& "�f�o�C�X�̏��̎擾�Ɏ��s���܂����@@Input::GetDeviceName");

	return Input::GetDeviceType(deviceInstance);

}

DeviceType mtgb::Input::GetDeviceType(const DIDEVICEINSTANCE& _inst)
{
	DeviceType deviceType = DeviceType::Unknown;

	//REF:https://learn.microsoft.com/ja-jp/previous-versions/windows/desktop/ee416610(v=vs.85)?devlangs=cpp&f1url=%3FappId%3DDev17IDEF1%26l%3DJA-JP%26k%3Dk(DINPUT%2FDIDEVICEINSTANCE)%3Bk(DIDEVICEINSTANCE)%3Bk(DevLang-C%2B%2B)%3Bk(TargetOS-Windows)%26rd%3Dtrue
	// ���ʃr�b�g�Ńf�o�C�X�̑�܂��ȃ^�C�v�𔻕�
	// ��ʃr�b�g�Ńf�o�C�X�̃T�u�^�C�v�����ʂł����
	DWORD major = _inst.dwDevType & 0xFF;
	switch (major)
	{
	case DI8DEVTYPE_FLIGHT:
		deviceType = DeviceType::FlightStick;
		break;
	case DI8DEVTYPE_GAMEPAD:
	case DI8DEVTYPE_JOYSTICK:
	case DI8DEVTYPE_1STPERSON:
		deviceType = DeviceType::GamePad;
		break;
	default:
		deviceType = DeviceType::Unknown;
		break;
	}

	return deviceType;
}

HRESULT mtgb::Input::UpdateJoystickState(GUID guid)
{
	return E_NOTIMPL;
}


int mtgb::Input::FindReservationIndexForDevice(DeviceType _devType) const
{
	int firstUnknown = -1;
	for (size_t i = 0; i < requestedJoystickDevices_.size(); i++)
	{
		const auto& reservation = requestedJoystickDevices_[i];
		if (reservation.deviceType == _devType)
		{
			return static_cast<int>(i);
		}
		if (reservation.deviceType == DeviceType::Unknown && firstUnknown < 0)
		{
			firstUnknown = static_cast<int>(i);
		}
	}
		return firstUnknown;
}

const std::string mtgb::Input::GetJoystickStatusMessage(GUID guid) const
{
	const auto& itr = joystickContext_.find(guid);
	if (itr == joystickContext_.end())
	{
		return "������";
	}
	return ConvertHResultToMessage(itr->second.lastResult);
}

bool mtgb::Input::IsJoystickConnected(GUID guid) const
{
	const auto& itr = joystickContext_.find(guid);
	if (itr == joystickContext_.end())
	{
		return false;
	}
	switch (itr->second.lastResult)
	{
	case DI_OK:
	case S_FALSE:
		return true;
	default:
		return false;
	}
}

bool mtgb::Input::IsJoystickAssigned(GUID guid) const
{
	return (joystickContext_.find(guid) != joystickContext_.end());
}

void mtgb::Input::StartEnumTimer()
{
	if (!enumTimerHandle_ || IsNotSubscribed())
		return;

	enumTimerHandle_ = Timer::AddInterval(enumInterval_, [this]() {AutoEnum(); });
}

void mtgb::Input::StopEnumTimer()
{
	if (!enumTimerHandle_) return;
	Timer::Remove(enumTimerHandle_);
	enumTimerHandle_ = nullptr;
}

void mtgb::Input::AutoEnum()
{
	if (IsNotSubscribed())
	{
		StopEnumTimer();
		return;
	}
	EnumJoystick();
}

void mtgb::Input::SetProperty(ComPtr<IDirectInputDevice8> _pJoystickDevice, InputConfig _inputConfig)
{
	HRESULT hResult{};

#pragma region �����[�h�ݒ�

	DIPROPDWORD diprop;
	diprop.diph.dwSize = sizeof(diprop);
	diprop.diph.dwHeaderSize = sizeof(diprop.diph);
	diprop.diph.dwHow = DIPH_DEVICE;

	//https://learn.microsoft.com/ja-jp/previous-versions/windows/desktop/ee416636(v=vs.85)
	//dwHow��DIPH_DEVICE�̏ꍇ��0�ɂ��Ȃ��Ƃ����Ȃ�
	diprop.diph.dwObj = 0;

	//REL:�O��̃f�o�C�X�Ƃ̑��Βl���g�p����
	//ABS:�f�o�C�X��̐�Βl���g�p����
	diprop.dwData = DIPROPAXISMODE_ABS;

	hResult = _pJoystickDevice->SetProperty(DIPROP_AXISMODE, &diprop.diph);
	massert(SUCCEEDED(hResult)
		&& "�����[�h�̐ݒ�Ɏ��s");

#pragma endregion

#pragma region �l�͈̔͐ݒ�

	DIPROPRANGE diprg;
	diprg;
	diprg.diph.dwSize = sizeof(diprg);
	diprg.diph.dwHeaderSize = sizeof(diprg.diph);
	diprg.diph.dwHow = DIPH_BYOFFSET;

	//X��
	diprg.diph.dwObj = DIJOFS_X;
	diprg.lMin = -_inputConfig.xRange;
	diprg.lMax = _inputConfig.xRange;

	hResult = _pJoystickDevice->SetProperty(DIPROP_RANGE, &diprg.diph);
	massert(SUCCEEDED(hResult)
		&& "�l�͈̔͐ݒ�Ɏ��s @");

	//y��
	diprg.diph.dwObj = DIJOFS_Y;
	diprg.lMin = -_inputConfig.yRange;
	diprg.lMax = _inputConfig.yRange;

	hResult = _pJoystickDevice->SetProperty(DIPROP_RANGE, &diprg.diph);
	massert(SUCCEEDED(hResult)
		&& "�l�͈̔͐ݒ�Ɏ��s @");

	//z��
	/*diprg.diph.dwObj = DIJOFS_Z;
	diprg.lMin = -_inputConfig.zRange;
	diprg.lMax = _inputConfig.zRange;

	hResult = _pJoystickDevice->SetProperty(DIPROP_RANGE, &diprg.diph);
	massert(SUCCEEDED(hResult)
		&& "�l�͈̔͐ݒ�Ɏ��s @");*/

#pragma endregion
}

mtgb::JoystickContext::JoystickContext()
	:timerHandle{nullptr}
{}

mtgb::JoystickContext::~JoystickContext()
{
	if (timerHandle)
	{
		Timer::Remove(timerHandle);
	}
	device.Reset();
}

mtgb::JoystickContext::JoystickContext(ComPtr<IDirectInputDevice8> _device)
	:JoystickContext()
{
	device = _device;
	deviceType = Input::GetDeviceType(device);
}

mtgb::JoystickReservation::~JoystickReservation()
{
}
