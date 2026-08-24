#include "InputResource.h"
#include "Input/InputData.h"
#include "Utility/ReleaseUtility.h"
#include "WindowContextUtil.h"
#include "Input/IncludingInput.h"
#include "Input/JoystickProxy.h"
#include "Editor/MTImGui.h"
using namespace mtgb;

mtgb::InputResource::InputResource(WindowContext _windowContext)
	: WindowContextResource(_windowContext)
	, pInputData_ { nullptr }
	, pKeyDevice_ { nullptr }
	, pMouseDevice_ { nullptr }
	, pJoystickProxy_ { nullptr }
	, pMouseStateProxy_ { nullptr }
	, assignedJoystickGuid_ { GUID_NULL }
	, isInitialized { false }
{
	HWND hWnd = WinCtxRes::GetHWND(_windowContext);

	// キーボードの取得
	Game::System<Input>().CreateKeyDevice(hWnd, pKeyDevice_.ReleaseAndGetAddressOf());

	// マウスの取得
	Game::System<Input>().CreateMouseDevice(hWnd, pMouseDevice_.ReleaseAndGetAddressOf());

	// 入力状態を保持するデータ
	pInputData_ = new InputData();

	// ImGui表示用のプロキシ
	pMouseStateProxy_ = new MouseStateProxy(pInputData_->mouseStateCurrent_);
	pJoystickProxy_	  = new JoystickProxy(pInputData_->joyStateCurrent_);

	// 入力の時間範囲を設定
	pInputData_->config_.SetRange(1000);
	pInputData_->config_.SetDeadZone(0.1f);
	pInputData_->config_.SetMaxMouseMovement(1.0f);

	JoystickReservation reservation;
	reservation.config = pInputData_->config_;
	reservation.hWnd   = hWnd;

	if (_windowContext == WindowContext::FIRST)
	{
		reservation.deviceType = DeviceType::UNKNOWN;
		name_				   = "FirstWindowController";
	}
	else if (_windowContext == WindowContext::SECOND)
	{
		reservation.deviceType = DeviceType::GAME_PAD;
		name_				   = "SecondWindowController";
	}

	reservation.onAssign = [this](ComPtr<IDirectInputDevice8> _device, GUID _guid)
	{
		pJoystickDevice_	  = _device;
		assignedJoystickGuid_ = _guid;
		isInitialized		  = true;
		Game::System<Input>().SetJoystickGuid(assignedJoystickGuid_);
		pInputData_->controllerType_ = Input::GetControllerTypeByVendor(pJoystickDevice_);
	};

	Game::System<Input>().RequestJoystickDevice(std::move(reservation));

	Game::System<Input>().EnumJoystick();
}

void mtgb::InputResource::Update()
{
	pJoystickProxy_->UpdateFromInput(assignedJoystickGuid_);
	pJoystickProxy_->UpdateInputData(pInputData_->joyStateCurrent_);

	MTImGui::TypedShow<JoystickProxy>(pJoystickProxy_, name_ + ":Joystick", ShowType::SETTINGS);

	pMouseStateProxy_->UpdateInputData(pInputData_->mouseStateCurrent_);
	MTImGui::TypedShow<MouseStateProxy>(pMouseStateProxy_, name_ + ":Mouse", ShowType::SETTINGS);

	UpdateStickTiltState();
}

void InputResource::SetResource()
{
	Input& input = Game::System<Input>();
	input.ChangeKeyDevice(pKeyDevice_);
	input.ChangeMouseDevice(pMouseDevice_);
	if (isInitialized)
	{
		input.SetJoystickGuid(assignedJoystickGuid_);
	}
	else
	{
		input.SetJoystickGuid(GUID_NULL);
	}
	input.ChangeInputData(pInputData_);
}

void mtgb::InputResource::Release()
{
	SAFE_DELETE(pInputData_);
	SAFE_DELETE(pJoystickProxy_);
	pKeyDevice_.Reset();
	pMouseDevice_.Reset();
	pJoystickDevice_.Reset();
}

void mtgb::InputResource::UpdateStickTiltState()
{
	pInputData_->stickTiltStates_[0].isFullyTiltedPrev_ = pInputData_->stickTiltStates_[0].isFullyTiltedCurr_;

	Vector2F axisPrev = InputUtil::GetAxis(StickType::LEFT, windowContext_);
	pInputData_->stickTiltStates_[0].isFullyTiltedCurr_ =
		(axisPrev.x == 1.0f) || (axisPrev.y == 1.0f) || (axisPrev.x == -1.0f) || (axisPrev.y == -1.0f);

	pInputData_->stickTiltStates_[1].isFullyTiltedPrev_ = pInputData_->stickTiltStates_[1].isFullyTiltedCurr_;
	Vector2F axisCurr									= InputUtil::GetAxis(StickType::RIGHT, windowContext_);
	pInputData_->stickTiltStates_[1].isFullyTiltedCurr_ =
		(axisCurr.x == 1.0f) || (axisCurr.y == 1.0f) || (axisCurr.x == -1.0f) || (axisCurr.y == -1.0f);
}
