#include "Input.h"
#include "Window/IncludingWindows.h"
#include "IncludingInput.h"
#include "InputData.h"
#include "Utility/MTAssert.h"
#include <algorithm>
#include "Core/Game.h"
#include "Core/SceneSystem.h"
#include "Debug.h"
#include "InputQuery.h"

namespace
{
	/// <summary>
	/// キーボードの状態取得用のバッファのサイズ
	/// </summary>
	constexpr size_t KEY_BUFFER_SIZE { 256 };
	/// <summary>
	/// DualShockのベンダーID
	/// </summary>
	constexpr DWORD VENDOR_ID_DUAL_SHOCK { 0x54c };
	/// <summary>
	/// XboxコントローラーのベンダーID
	/// </summary>
	constexpr DWORD VENDOR_ID_XBOX { 0x45E };
	/// <summary>
	/// HORIのベンダーID
	/// </summary>
	constexpr DWORD VENDOR_ID_HORI { 0x0F0D };
	/// <summary>
	/// HORIパッドFPSプラス for PlayStation4のプロダクトID
	/// </summary>
	constexpr DWORD PRODUCT_ID_HORI_PAD_PS4 { 0x0066 };

} // namespace

using namespace mtgb;

void mtgb::Input::AcquireController(ComPtr<IDirectInputDevice8> _pControllerDevice)
{
	// コントローラーの取得を試みる

	HRESULT hResult											  = _pControllerDevice->Acquire();
	controllerContext_[currControllerGuid_].lastAcquireResult = hResult;

	// 取得を試みた結果に応じて処理をする
	switch (hResult)
	{
		// ログを出す
		case DI_OK :   // 取得できた
		case S_FALSE : // 他のアプリも許可を取得している
			LOGIMGUI("Acquire Joystick");
			break;
		// 何もしない
		case DIERR_OTHERAPPHASPRIO : // 他のアプリが優先権を持っている
			break;
		// エラー発生
		case DIERR_INVALIDPARAM :
		case DIERR_NOTINITIALIZED :
			massert(SUCCEEDED(hResult) && "コントローラー操作の許可取得の際にエラーが起こりました @Input::Update");
			break;
		default :
			break;
	}
}

GUID mtgb::Input::GetDeviceGuid(ComPtr<IDirectInputDevice8> _pInputDevice)
{
	// デバイスのGUIDを取得
	DIDEVICEINSTANCE deviceInstance = {};
	deviceInstance.dwSize			= sizeof(DIDEVICEINSTANCE);
	HRESULT hResult					= _pInputDevice->GetDeviceInfo(&deviceInstance);
	massert(SUCCEEDED(hResult) && "デバイスの情報の取得に失敗しました @Input::Update");
	return deviceInstance.guidInstance;
}

bool operator<(const GUID& _lhs, const GUID& _rhs)
{
	return std::memcmp(&_lhs, &_rhs, sizeof(GUID)) < 0;
}

mtgb::Input::Input()
	: pInputData_ { nullptr }
	, pDirectInput_ { nullptr }
	, pKeyDevice_ { nullptr }
	, pMouseDevice_ { nullptr }
	, currControllerGuid_ { 0 }
{
}

mtgb::Input::~Input()
{
	Release();
}

void mtgb::Input::Initialize()
{
	HRESULT hResult {};

	// DirectInput8のデバイス作成
	hResult = DirectInput8Create(
		GetModuleHandle(nullptr),
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		reinterpret_cast<void**>(pDirectInput_.GetAddressOf()),
		nullptr
	);

	massert(
		SUCCEEDED(hResult) // DirectInput8のデバイス作成に成功
		&& "DirectInput8のデバイス作成に失敗 @Input::Initialize"
	);
}

void mtgb::Input::Update()
{
	HWND activeWndH = GetForegroundWindow();
	if (!activeWndH)
		return;

	DWORD activeProcessId = 0;
	GetWindowThreadProcessId(activeWndH, &activeProcessId);

	if (activeProcessId != GetCurrentProcessId())
	{
		if (pInputData_)
		{
			pInputData_->Initialize();
		}
		return;
	}

	// デバイスの入力状態更新
	UpdateKeyDevice();
	UpdateMouseDevice();
	UpdateControllerDevice();

	// コントローラーの取得を試みる
	if (InputQuery::GetKeyDown(KeyCode::P))
	{
		EnumController();
	}
}

void mtgb::Input::UpdateKeyDevice()
{
	// キーボード操作の許可取得を試みる
	HRESULT hResult = pKeyDevice_->Acquire();

	// キーボード操作の許可取得に失敗した場合
	if (FAILED(hResult))
		return;

	// キー状態取得用バッファ
	BYTE keyBuffer[KEY_BUFFER_SIZE] {};

	// 現在のキーの状態を記録
	pInputData_->keyStatePrevious_ = pInputData_->keyStateCurrent_;

	// キーの状態を取得してバッファに入れる
	pKeyDevice_->GetDeviceState(KEY_BUFFER_SIZE, keyBuffer);

	// キーの状態をバッファから取り出す
	for (int i = 0; i < KEY_BUFFER_SIZE; i++)
	{
		pInputData_->keyStateCurrent_[i] = keyBuffer[i];
	}
}

void mtgb::Input::UpdateMouseDevice()
{
	// マウス操作の許可取得を試みる
	HRESULT hResult = pMouseDevice_->Acquire();

	// マウス操作の許可取得に失敗した場合
	if (FAILED(hResult))
		return;

	massert(
		SUCCEEDED(hResult) // マウス操作の許可取得に成功
		&& "マウス操作の許可取得に失敗 @Input::Update"
	);

	// 現在のマウスの状態を記録
	memcpy(&pInputData_->mouseStatePrevious_, &pInputData_->mouseStateCurrent_, sizeof(DIMOUSESTATE));

	// マウスの状態を取得してバッファに入れる
	hResult = pMouseDevice_->GetDeviceState(sizeof(DIMOUSESTATE), &pInputData_->mouseStateCurrent_);

	massert(
		SUCCEEDED(hResult) // マウス操作の取得に成功
		&& "マウス操作の取得に失敗 @Input::Update"
	);
}

void mtgb::Input::UpdateControllerDevice()
{
	// コントローラーが登録されていない場合
	if (controllerContext_.empty())
		return;
	// 現在のコントローラーのGUIDがnullの場合
	if (currControllerGuid_ == GUID_NULL)
		return;

	// 現在のコントローラーの状態を記録
	memcpy(&pInputData_->joyStatePrevious_, &pInputData_->joyStateCurrent_, sizeof(DIJOYSTATE));

	// コントローラーの状態を取得
	HRESULT hResult = controllerContext_[currControllerGuid_].device->GetDeviceState(
		sizeof(DIJOYSTATE),
		&pInputData_->joyStateCurrent_
	);
	// 取得結果を記録
	controllerContext_[currControllerGuid_].lastAcquireResult = hResult;

	// 取得結果に応じた処理
	switch (hResult)
	{
		// 成功時
		case DI_OK :
			break;
		// 入力ロスト、一時的なアクセス不可の場合
		case DIERR_INPUTLOST :
			// 取得を試みる
			AcquireController(controllerContext_[currControllerGuid_].device);
			return;
		// 未取得の場合
		case DIERR_NOTACQUIRED :
			// 取得を試みる
			AcquireController(controllerContext_[currControllerGuid_].device);
			return;
			// 何らかの失敗の場合
		default :
		{
			// デバイスを割り当て済みリストから除外
			UnregisterControllerGuid(GetDeviceGuid(controllerContext_[currControllerGuid_].device));
			return;
		}
	}
}

void mtgb::Input::Release()
{
	// デバイスを解放
	pMouseDevice_.Reset();
	pKeyDevice_.Reset();
	pConrtollerDevice_.Reset();
	pDirectInput_.Reset();
}

void mtgb::Input::UpdateMousePositionData(int32_t _x, int32_t _y)
{
	// マウスの座標データを更新
	if (pInputData_)
	{
		pInputData_->mousePosition_.x = _x;
		pInputData_->mousePosition_.y = _y;
	}
}

void mtgb::Input::CreateKeyDevice(HWND _hWnd, LPDIRECTINPUTDEVICE8* _ppKeyDevice)
{
	// キーボードデバイスを作成
	HRESULT hResult = pDirectInput_->CreateDevice(GUID_SysKeyboard, _ppKeyDevice, nullptr);
	massert(
		SUCCEEDED(hResult) // キーボードデバイスの作成に成功
		&& "キーボードデバイスの作成に失敗 @Input::CreateKeyDevice"
	);

	// キーボード用にフォーマット
	hResult = (*_ppKeyDevice)->SetDataFormat(&c_dfDIKeyboard);

	massert(
		SUCCEEDED(hResult) // キーボードフォーマットに成功
		&& "キーボードフォーマットに失敗 @Input::CreateDevice"
	);

	// キーボードのアプリ間共有レベルを設定
	//  REF: https://learn.microsoft.com/ja-jp/previous-versions/windows/desktop/ee417921(v=vs.85)
	// hResult = (*_ppKeyDevice)->SetCooperativeLevel(_hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	// 非アクティブなアプリも入力を受け付ける
	hResult = (*_ppKeyDevice)->SetCooperativeLevel(_hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);

	massert(
		SUCCEEDED(hResult) // キーボードアプリ間共有レベル設定に成功
		&& "キーボードアプリ間共有レベル設定に失敗 @Input::CreateDevice"
	);
}

void mtgb::Input::CreateMouseDevice(HWND _hWnd, LPDIRECTINPUTDEVICE8* _ppMouseDevice)
{
	HRESULT hResult {};

	hResult = pDirectInput_->CreateDevice(GUID_SysMouse, _ppMouseDevice, nullptr);
	massert(
		SUCCEEDED(hResult) // キーボードデバイスの作成に成功
		&& "マウスデバイスの作成に失敗 @Input::CreateMouseDevice"
	);

	// マウス用にフォーマット
	hResult = (*_ppMouseDevice)->SetDataFormat(&c_dfDIMouse);

	massert(
		SUCCEEDED(hResult) // マウスフォーマットに成功
		&& "マウスフォーマットに失敗 @Input::CreateMouseDevice"
	);

	// マウスのアプリ間共有レベルの設定
	// hResult = (*_ppMouseDevice)->SetCooperativeLevel(_hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	// 非アクティブなアプリも入力を受け付ける
	hResult = (*_ppMouseDevice)->SetCooperativeLevel(_hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);

	massert(
		SUCCEEDED(hResult) // マウスアプリ間共有レベル設定に成功
		&& "マウスアプリ間共有レベル設定に失敗 @Input::CreateMouseDevice"
	);
}

void mtgb::Input::ChangeKeyDevice(ComPtr<IDirectInputDevice8> _pKeyDevice)
{
	pKeyDevice_ = _pKeyDevice;
}

void mtgb::Input::SetControllerGuid(GUID _guid)
{
	currControllerGuid_ = _guid;
}

void mtgb::Input::ChangeMouseDevice(ComPtr<IDirectInputDevice8> _pMouseDevice)
{
	pMouseDevice_ = _pMouseDevice;
}

void mtgb::Input::ChangeInputData(InputData* _pInputData)
{
	pInputData_ = _pInputData;
}

void mtgb::Input::ChangeControllerDevice(ComPtr<IDirectInputDevice8> _pJoystickDevice)
{
	pConrtollerDevice_ = _pJoystickDevice;
}

/// <summary>
/// コントローラーが接続されている場合、デバイスに割り当てる
/// </summary>
/// <param name="lpddi">デバイスの情報を持つインスタンス</param>
/// <param name="pvRef">EnumDevicesで渡した値</param>
/// <returns></returns>
BOOL CALLBACK EnumControllerCallback(const LPCDIDEVICEINSTANCE _lpddi, LPVOID _pvRef)
{
	auto& input = Game::System<Input>();

	// 割り当て予約がなかったらデバイスを作成しない
	if (input.IsNotSubscribed())
	{
		return DIENUM_STOP;
	}
	ControllerType devType = Input::GetControllerType(*_lpddi);

	int reservationIndex = input.FindReservationIndexForDevice(devType);

	if (reservationIndex < 0)
	{
		// デバイスを要求している予約はない
		return DIENUM_CONTINUE;
	}
	LPDIRECTINPUT8 pDirectInput			= reinterpret_cast<LPDIRECTINPUT8>(_pvRef);
	ComPtr<IDirectInputDevice8> pDevice = nullptr;

	// 割り当て予約があり、未割当なのでデバイス作成
	HRESULT hResult = pDirectInput->CreateDevice(_lpddi->guidInstance, pDevice.GetAddressOf(), nullptr);
	massert(SUCCEEDED(hResult) && "コントローラーのデバイスの作成に失敗 @EnumJoysticksCallback");

	if (Game::System<Input>().RegisterControllerGuid(_lpddi->guidInstance) == false)
	{
		// 既に割り当て済みの為、他のデバイスの列挙に移す
		pDevice.Reset();
		return DIENUM_CONTINUE;
	}

	input.AssignControllerToReservation(pDevice, static_cast<size_t>(reservationIndex), _lpddi->guidInstance);
	LOGIMGUI_CAT("Input", "Assigned reservationIndex=%d", reservationIndex);

	// 予約がまだ残っているなら続行
	return input.IsNotSubscribed() ? DIENUM_STOP : DIENUM_CONTINUE;
}

void mtgb::Input::EnumController()
{
	// 割り当て予約がなかったらデバイスを作成しない
	if (Game::System<Input>().IsNotSubscribed())
	{
		return;
	}
	pDirectInput_->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumControllerCallback, pDirectInput_.Get(), DIEDFL_ATTACHEDONLY);
}

void mtgb::Input::RequestControllerDevice(const ControllerReservation& _reservation)
{
	requestedControllerDevices_.push_back(_reservation);
}

void mtgb::Input::RequestControllerDevice(ControllerReservation&& _reservation)
{
	requestedControllerDevices_.push_back(std::move(_reservation));
}

void mtgb::Input::AssignControllerToReservation(
	ComPtr<IDirectInputDevice8> _pControllerDevice,
	size_t _reservationIndex,
	GUID _guid
)
{
	if (_reservationIndex >= requestedControllerDevices_.size())
		return;

	// 予約をムーブ
	ControllerReservation reservation = std::move(requestedControllerDevices_[_reservationIndex]);
	requestedControllerDevices_.erase(requestedControllerDevices_.begin() + _reservationIndex);

	// 協調レベル等設定
	_pControllerDevice->SetCooperativeLevel(reservation.hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
	//_pControllerDevice->SetCooperativeLevel(reservation.hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	_pControllerDevice->SetDataFormat(&c_dfDIJoystick);
	SetProperty(_pControllerDevice, reservation.config);

	// デバイスからJoystickContext構築
	GUID guid			 = GetDeviceGuid(_pControllerDevice);
	auto [itr, inserted] = controllerContext_.emplace(guid, _pControllerDevice);

	if (reservation.onAssign)
		reservation.onAssign(itr->second.device, guid);
}

void mtgb::Input::UnregisterControllerGuid(GUID _guid)
{
	controllerContext_.erase(_guid);
}

bool mtgb::Input::RegisterControllerGuid(GUID _guid)
{
	return assignedControllerGuids_.insert(_guid).second;
}

bool mtgb::Input::IsNotSubscribed()
{
	return requestedControllerDevices_.empty();
}

GamePadType mtgb::Input::GetGamePadTypeByVendor(ComPtr<IDirectInputDevice8> _pInputDevice)
{
	DIDEVICEINSTANCE deviceInstance = {};
	deviceInstance.dwSize			= sizeof(DIDEVICEINSTANCE);
	HRESULT hResult					= _pInputDevice->GetDeviceInfo(&deviceInstance);
	if (FAILED(hResult))
		return GamePadType::UNKNOWN;

	// プロダクトID
	DWORD productId = HIWORD(deviceInstance.guidProduct.Data1);
	// ベンダーID
	DWORD vendorId = LOWORD(deviceInstance.guidProduct.Data1);

	// デュアルショック
	if (vendorId == VENDOR_ID_DUAL_SHOCK)
	{
		return GamePadType::DUAL_SHOCK;
	}
	// ホリパッドFPSプラス for PlayStation4
	if (vendorId == VENDOR_ID_HORI)
	{
		if (productId == PRODUCT_ID_HORI_PAD_PS4)
		{
			return GamePadType::DUAL_SHOCK;
		}
	}

	// Xboxコントローラー
	if (vendorId == VENDOR_ID_XBOX)
	{
		return GamePadType::XBOX;
	}

	return GamePadType::UNKNOWN;
}

std::string mtgb::Input::GetDeviceName(ComPtr<IDirectInputDevice8> _pInputDevice)
{
	DIDEVICEINSTANCE deviceInstance = {};
	deviceInstance.dwSize			= sizeof(DIDEVICEINSTANCE);
	HRESULT hResult					= _pInputDevice->GetDeviceInfo(&deviceInstance);
	massert(SUCCEEDED(hResult) && "デバイスの情報の取得に失敗しました　@Input::GetDeviceName");

	return std::string(deviceInstance.tszInstanceName);
}

std::string mtgb::Input::GetDeviceName(GUID _guid)
{
	if (controllerContext_.contains(_guid))
	{
		return GetDeviceName(controllerContext_[_guid].device);
	}
	return "None";
}

std::string mtgb::Input::GetDeviceProductName(ComPtr<IDirectInputDevice8> _pInputDevice)
{
	DIDEVICEINSTANCE deviceInstance = {};
	deviceInstance.dwSize			= sizeof(DIDEVICEINSTANCE);
	HRESULT hResult					= _pInputDevice->GetDeviceInfo(&deviceInstance);
	massert(SUCCEEDED(hResult) && "デバイスの情報の取得に失敗しました　@Input::GetDeviceName");

	return std::string(deviceInstance.tszProductName);
}

std::string mtgb::Input::GetDeviceProductName(GUID _guid)
{
	if (controllerContext_.contains(_guid))
	{
		return GetDeviceProductName(controllerContext_[_guid].device);
	}
	return "None";
}

std::string_view mtgb::Input::ConvertHResultToMessage(HRESULT _hr) const
{
	switch (_hr)
	{
		case DI_OK :
			return "取得";
		case S_FALSE :
			return "他アプリと共有";
		case DIERR_INPUTLOST :
			return "切断";
		case DIERR_NOTACQUIRED :
			return "デバイス未取得";
		case DIERR_OTHERAPPHASPRIO :
			return "他が優先権を所持";
		default :
			return "不明なエラー";
	}
}

ControllerType mtgb::Input::GetControllerType(ComPtr<IDirectInputDevice8> _pControllerDevice)
{
	DIDEVICEINSTANCE deviceInstance = {};
	deviceInstance.dwSize			= sizeof(DIDEVICEINSTANCE);
	HRESULT hResult					= _pControllerDevice->GetDeviceInfo(&deviceInstance);

	massert(SUCCEEDED(hResult) && "デバイスの情報の取得に失敗しました　@Input::GetDeviceName");

	return Input::GetControllerType(deviceInstance);
}

ControllerType mtgb::Input::GetControllerType(const DIDEVICEINSTANCE& _inst)
{
	ControllerType controllerType = ControllerType::UNKNOWN;

	// REF:https://learn.microsoft.com/ja-jp/previous-versions/windows/desktop/ee416610(v=vs.85)?devlangs=cpp&f1url=%3FappId%3DDev17IDEF1%26l%3DJA-JP%26k%3Dk(DINPUT%2FDIDEVICEINSTANCE)%3Bk(DIDEVICEINSTANCE)%3Bk(DevLang-C%2B%2B)%3Bk(TargetOS-Windows)%26rd%3Dtrue
	//  下位ビットでデバイスの大まかなタイプを判別
	//  上位ビットでデバイスのサブタイプも判別できるよ
	DWORD major = _inst.dwDevType & 0xFF;
	switch (major)
	{
		case DI8DEVTYPE_FLIGHT :
			controllerType = ControllerType::FLIGHT_STICK;
			break;
		case DI8DEVTYPE_GAMEPAD :
		case DI8DEVTYPE_JOYSTICK :
		case DI8DEVTYPE_1STPERSON :
			controllerType = ControllerType::GAME_PAD;
			break;
		default :
			controllerType = ControllerType::UNKNOWN;
			break;
	}

	return controllerType;
}

int mtgb::Input::FindReservationIndexForDevice(ControllerType _devType) const
{
	int firstUnknown = -1;
	for (size_t i = 0; i < requestedControllerDevices_.size(); i++)
	{
		const auto& reservation = requestedControllerDevices_[i];
		if (reservation.controllerType == _devType)
		{
			return static_cast<int>(i);
		}
		if (reservation.controllerType == ControllerType::UNKNOWN && firstUnknown < 0)
		{
			firstUnknown = static_cast<int>(i);
		}
	}
	return firstUnknown;
}

std::string_view mtgb::Input::GetControllerStatusMessage(GUID _guid) const
{
	const auto& itr = controllerContext_.find(_guid);
	if (itr == controllerContext_.end())
	{
		return "未割当";
	}
	return ConvertHResultToMessage(itr->second.lastAcquireResult);
}

void mtgb::Input::SetProperty(ComPtr<IDirectInputDevice8> _pJoystickDevice, InputConfig _inputConfig)
{
	HRESULT hResult {};

#pragma region 軸モード設定

	DIPROPDWORD diprop;
	diprop.diph.dwSize		 = sizeof(diprop);
	diprop.diph.dwHeaderSize = sizeof(diprop.diph);
	diprop.diph.dwHow		 = DIPH_DEVICE;

	// https://learn.microsoft.com/ja-jp/previous-versions/windows/desktop/ee416636(v=vs.85)
	// dwHowがDIPH_DEVICEの場合は0にしないといけない
	diprop.diph.dwObj = 0;

	// REL:前回のデバイスとの相対値を使用する
	// ABS:デバイス上の絶対値を使用する
	diprop.dwData = DIPROPAXISMODE_ABS;

	hResult = _pJoystickDevice->SetProperty(DIPROP_AXISMODE, &diprop.diph);
	massert(SUCCEEDED(hResult) && "軸モードの設定に失敗");

#pragma endregion

#pragma region 値の範囲設定

	DIPROPRANGE diprg;
	diprg;
	diprg.diph.dwSize		= sizeof(diprg);
	diprg.diph.dwHeaderSize = sizeof(diprg.diph);
	diprg.diph.dwHow		= DIPH_BYOFFSET;

	// 左スティック、X軸
	diprg.diph.dwObj = DIJOFS_X;
	diprg.lMin		 = -_inputConfig.xRange;
	diprg.lMax		 = _inputConfig.xRange;

	hResult = _pJoystickDevice->SetProperty(DIPROP_RANGE, &diprg.diph);
	massert(SUCCEEDED(hResult) && "値の範囲設定に失敗 @");

	// 左スティック、Y軸
	diprg.diph.dwObj = DIJOFS_Y;
	diprg.lMin		 = -_inputConfig.yRange;
	diprg.lMax		 = _inputConfig.yRange;

	hResult = _pJoystickDevice->SetProperty(DIPROP_RANGE, &diprg.diph);
	massert(SUCCEEDED(hResult) && "値の範囲設定に失敗 @");

	GamePadType controllerType = GetGamePadTypeByVendor(_pJoystickDevice);
	switch (controllerType)
	{
		case GamePadType::DUAL_SHOCK :
			// 右スティック、X軸
			diprg.diph.dwObj = DIJOFS_Z;
			diprg.lMin		 = -_inputConfig.xRange;
			diprg.lMax		 = _inputConfig.xRange;

			hResult = _pJoystickDevice->SetProperty(DIPROP_RANGE, &diprg.diph);
			massert(SUCCEEDED(hResult) && "値の範囲設定に失敗 @");

			// 右スティック、Y軸
			diprg.diph.dwObj = DIJOFS_RZ;
			diprg.lMin		 = -_inputConfig.yRange;
			diprg.lMax		 = _inputConfig.yRange;

			hResult = _pJoystickDevice->SetProperty(DIPROP_RANGE, &diprg.diph);
			massert(SUCCEEDED(hResult) && "値の範囲設定に失敗");
			break;
		case GamePadType::XBOX :
			// 右スティック、X軸
			diprg.diph.dwObj = DIJOFS_RX;
			diprg.lMin		 = -_inputConfig.xRange;
			diprg.lMax		 = _inputConfig.xRange;

			hResult = _pJoystickDevice->SetProperty(DIPROP_RANGE, &diprg.diph);
			massert(SUCCEEDED(hResult) && "値の範囲設定に失敗");

			// 左スティック、y軸
			diprg.diph.dwObj = DIJOFS_RY;
			diprg.lMin		 = -_inputConfig.yRange;
			diprg.lMax		 = _inputConfig.yRange;

			hResult = _pJoystickDevice->SetProperty(DIPROP_RANGE, &diprg.diph);
			massert(SUCCEEDED(hResult) && "値の範囲設定に失敗");
			break;

		default :
			// 不明な場合は両方設定してしまう
			// DualShock設定
			diprg.diph.dwObj = DIJOFS_Z;
			_pJoystickDevice->SetProperty(DIPROP_RANGE, &diprg.diph);
			diprg.diph.dwObj = DIJOFS_RZ;
			_pJoystickDevice->SetProperty(DIPROP_RANGE, &diprg.diph);

			// Xbox設定
			diprg.diph.dwObj = DIJOFS_RX;
			_pJoystickDevice->SetProperty(DIPROP_RANGE, &diprg.diph);
			diprg.diph.dwObj = DIJOFS_RY;
			_pJoystickDevice->SetProperty(DIPROP_RANGE, &diprg.diph);
			break;
	}

#pragma endregion
}

mtgb::ControllerContext::ControllerContext()
	: lastAcquireResult { S_OK }
	, device { nullptr }
	, controllerType { ControllerType::UNKNOWN }
{
}

mtgb::ControllerContext::~ControllerContext()
{
	device.Reset();
}

mtgb::ControllerContext::ControllerContext(ComPtr<IDirectInputDevice8> _device)
	: ControllerContext()
{
	device		   = _device;
	controllerType = Input::GetControllerType(device);
}

mtgb::ControllerReservation::ControllerReservation()
	: hWnd { nullptr }
	, config {}
	, controllerType { ControllerType::UNKNOWN }
	, onAssign { nullptr }
{
}
