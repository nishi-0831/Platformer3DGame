#include "InputResource.h"
#include "Input/InputData.h"
#include "Input/InputQuery.h"
#include "Utility/ReleaseUtility.h"
#include "WindowContextUtil.h"
#include "Input/IncludingInput.h"
#include "Input/ControllerProxy.h"
#include "Editor/MTImGui.h"
using namespace mtgb;

mtgb::InputResource::InputResource(WindowContext _windowContext)
	: WindowContextResource(_windowContext)
	, pInputData_ { nullptr }
	, pKeyDevice_ { nullptr }
	, pMouseDevice_ { nullptr }
	, pControllerProxy_ { nullptr }
	, pMouseStateProxy_ { nullptr }
	, assignedControllerGuid_ { GUID_NULL }
	, isInitializedControllerDevice_ { false }
{
	// HWND取得
	HWND hWnd = WinCtxRes::GetHWND(_windowContext);

	// キーボードの取得
	Game::System<Input>().CreateKeyDevice(hWnd, pKeyDevice_.ReleaseAndGetAddressOf());

	// マウスの取得
	Game::System<Input>().CreateMouseDevice(hWnd, pMouseDevice_.ReleaseAndGetAddressOf());

	// 入力状態を保持するデータ
	pInputData_ = new InputData();

	// ImGui表示用のプロキシ
	pMouseStateProxy_ = new MouseStateProxy(pInputData_->mouseStateCurrent_);
	pControllerProxy_ = new ControllerProxy(pInputData_->joyStateCurrent_);

	// 入力の取り方を設定

	// 入力の範囲
	pInputData_->config_.SetRange(1000);
	// デッドゾーン
	pInputData_->config_.SetDeadZone(0.1f);
	// マウス移動量の上限
	pInputData_->config_.SetMaxMouseMovement(20.0f);

	// コントローラーの割り当て予約を作成していく
	ControllerReservation reservation;
	// 入力設定、HWNDを渡す
	reservation.config = pInputData_->config_;
	reservation.hWnd   = hWnd;
	// 割り当てて欲しいのはゲームパッド
	reservation.controllerType = ControllerType::GAME_PAD;
	name_					   = "Input Resource";

	// 割り当て時に呼ばれるコールバック
	reservation.onAssign = [this](ComPtr<IDirectInputDevice8> _device, GUID _guid)
	{
		// コントローラーデバイス、GUIDを保存
		pConrtollerDevice_		= _device;
		assignedControllerGuid_ = _guid;
		// 初期化したか否かのフラグをオン
		isInitializedControllerDevice_ = true;
		// ゲーム側で扱われるコントローラーとして設定
		Game::System<Input>().SetControllerGuid(assignedControllerGuid_);
		// ゲームパッドの種類を取得
		pInputData_->gamePadType_ = Input::GetGamePadTypeByVendor(pConrtollerDevice_);
	};

	// 割り当て予約を渡す
	Game::System<Input>().RequestControllerDevice(std::move(reservation));

	// コントローラーの列挙を行う
	Game::System<Input>().EnumController();
}

void mtgb::InputResource::Update()
{
	// ImGui表示用のプロキシを更新

	// コントローラーのプロキシ更新
	pControllerProxy_->UpdateFromInput(assignedControllerGuid_);
	pControllerProxy_->UpdateInputData(pInputData_->joyStateCurrent_);
	// マウスのプロキシ更新
	pMouseStateProxy_->UpdateInputData(pInputData_->mouseStateCurrent_);

	// コントローラー、マウスの入力状態を表示
	MTImGui::TypedShow<ControllerProxy>(pControllerProxy_, name_ + ":Controller", ShowType::SETTINGS);
	MTImGui::TypedShow<MouseStateProxy>(pMouseStateProxy_, name_ + ":Mouse", ShowType::SETTINGS);

	// スティックの傾き状態を更新
	UpdateStickTiltStates();
}

void InputResource::SetResource()
{
	Input& input = Game::System<Input>();

	// キーボード、マウスのデバイスを設定
	input.ChangeKeyDevice(pKeyDevice_);
	input.ChangeMouseDevice(pMouseDevice_);

	// コントローラーのデバイスを設定

	// 初期化されているなら、GUIDを設定
	if (isInitializedControllerDevice_)
	{
		input.SetControllerGuid(assignedControllerGuid_);
	}
	// されていないなら無効なGUID設定
	else
	{
		input.SetControllerGuid(GUID_NULL);
	}

	// 更新してほしい入力状態のデータを渡す
	input.ChangeInputData(pInputData_);
}

void mtgb::InputResource::Release()
{
	SAFE_DELETE(pInputData_);
	SAFE_DELETE(pControllerProxy_);
	pKeyDevice_.Reset();
	pMouseDevice_.Reset();
	pConrtollerDevice_.Reset();
}

void mtgb::InputResource::UpdateStickTiltStates()
{
	// 傾き状態を更新

	// 左スティック
	UpdateStickTiltState(pInputData_->stickTiltStates_[InputData::LEFT_STICK_INDEX], StickType::LEFT);

	// 右スティック
	UpdateStickTiltState(pInputData_->stickTiltStates_[InputData::RIGHT_STICK_INDEX], StickType::RIGHT);
}

void mtgb::InputResource::UpdateStickTiltState(InputData::StickTiltState& _state, StickType _stickType)
{
	// 現在の傾き状態を前回の状態にする
	_state.isFullyTiltedPrev_ = _state.isFullyTiltedCurr_;

	// 傾き具合を取得
	Vector2F axis = InputQuery::GetAxis(_stickType, windowContext_);

	// 完全に傾いているか判定
	_state.isFullyTiltedCurr_ = std::abs(axis.x) >= 1.0f || std::abs(axis.y) >= 1.0f;
}
