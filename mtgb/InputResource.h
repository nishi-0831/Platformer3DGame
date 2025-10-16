#pragma once
#include "WindowContextResource.h"
#include "WindowContextResourceManager.h"
#include "MTAssert.h"
#include "Game.h"
#include "ISystem.h"
#include "Input.h"
#include "WindowContext.h"
#include <map>
#include <typeindex>
#include "JoystickProxy.h"
#include "MouseStateProxy.h"
#include "Timer.h"
#include "InputConfig.h"
#include <string>
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dInput8.lib")
#pragma comment(lib, "Xinput.lib")

typedef struct HWND__* HWND;

namespace mtgb
{
	class InputData;
	class InputResource : public WindowContextResource
	{
	public:
		InputResource();
		~InputResource();
		InputResource(const InputResource& other);
		void Initialize(WindowContext _windowContext) override;
		void Update() override;
		void SetResource() override;
		void Release() override;
		const InputData* GetInput(){ return pInputData_; }
		InputData* pInputData_;
	private:
		std::string name_;
		ComPtr<IDirectInputDevice8> pKeyDevice_;
		ComPtr<IDirectInputDevice8> pMouseDevice_;
		ComPtr<IDirectInputDevice8> pJoystickDevice_;
		JoystickProxy* pJoystickProxy_;
		MouseStateProxy* pMouseStateProxy_;
		GUID assignedJoystickGuid_;
		bool isInitialized;
		// WindowContextResource ����Čp������܂���
		WindowContextResource* Clone() const override;
	};
}