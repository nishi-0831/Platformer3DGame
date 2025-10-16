#include "InputResource.h"
#include "InputData.h"
#include "ReleaseUtility.h"
#include "WindowContextUtil.h"
#include "IncludingInput.h"
#include "JoystickProxy.h"
#include "MTImGui.h"
#include "Debug.h"
using namespace mtgb;
namespace
{
	static int id = 0;
}
mtgb::InputResource::InputResource()
	: pInputData_{nullptr}
	, pKeyDevice_{nullptr}
	, pMouseDevice_{nullptr}
	, pJoystickProxy_{nullptr}
	, pMouseStateProxy_{nullptr}
	, assignedJoystickGuid_{GUID_NULL}
{
	
}

mtgb::InputResource::~InputResource()
{
	Release();
}

mtgb::InputResource::InputResource(const InputResource& other)
	: WindowContextResource(other)
	, pKeyDevice_{ other.pKeyDevice_ }       
	, pMouseDevice_{ other.pMouseDevice_ }
	, isInitialized{ other.isInitialized }
	, assignedJoystickGuid_{ other.assignedJoystickGuid_ }
{
	if (other.pInputData_)
	{
		pInputData_ = new InputData(*other.pInputData_);
	}
	else
	{
		pInputData_ = nullptr;
	}
	if (other.pJoystickProxy_)
	{
		pJoystickProxy_ = new JoystickProxy(*other.pJoystickProxy_);
	}
	else
	{
		pJoystickProxy_ = nullptr;
	}
	if (other.pMouseStateProxy_)
	{
		pMouseStateProxy_ = new MouseStateProxy{ *other.pMouseStateProxy_ };
	}
	else
	{
		pMouseStateProxy_ = nullptr;
	}
}

void mtgb::InputResource::Initialize(WindowContext _windowContext)
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
	pJoystickProxy_ = new JoystickProxy(pInputData_->joyStateCurrent_);

	// 入力の取り方を設定
	pInputData_->config_.SetRange(1000);
	pInputData_->config_.SetDeadZone(0.1);
	//pProxy_->SetDisplayName("proxy:"+ id++);
	
	
	JoystickReservation reservation; 
	reservation.config = pInputData_->config_;
	reservation.hWnd = hWnd;

	if (_windowContext == WindowContext::First)
	{
		reservation.deviceType = DeviceType::Unknown;
		name_ = "FirstWindowController";
	}
	else if (_windowContext == WindowContext::Second)
	{
		reservation.deviceType = DeviceType::GamePad;
		name_ = "SecondWindowController";
	}

	reservation.onAssign = [this](ComPtr<IDirectInputDevice8> device,GUID guid)
		{
			pJoystickDevice_ = device;
			assignedJoystickGuid_ = guid;
			isInitialized = true;
			Game::System<Input>().SetJoystickGuid(assignedJoystickGuid_);

		};

	Game::System<Input>().RequestJoystickDevice(std::move(reservation));

	Game::System<Input>().EnumJoystick();

}

void mtgb::InputResource::Update()
{
	Input& input = Game::System<Input>();
	
	pJoystickProxy_->UpdateFromInput(assignedJoystickGuid_);
	pJoystickProxy_->UpdateInputData(pInputData_->joyStateCurrent_);
	
	MTImGui::Instance().TypedShow<JoystickProxy>(pJoystickProxy_, name_+":Joystick", ShowType::Inspector);

	pMouseStateProxy_->UpdateInputData(pInputData_->mouseStateCurrent_);
	MTImGui::Instance().TypedShow<MouseStateProxy>(pMouseStateProxy_, name_ + ":Mouse", ShowType::Inspector);
	//LOGIMGUI("%ld", pMouseStateProxy_->lZ);
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

WindowContextResource* mtgb::InputResource::Clone() const
{
	return new InputResource(*this);
}

