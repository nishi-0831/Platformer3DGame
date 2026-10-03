#pragma once
#include "WindowContextResource.h"
#include "WindowContextResourceManager.h"
#include "Input/Input.h"
#include "Input/InputData.h"
#include "WindowContext.h"
#include "Input/ControllerProxy.h"
#include "Input/MouseStateProxy.h"
#include <string>
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dInput8.lib")
#pragma comment(lib, "Xinput.lib")

typedef struct HWND__* HWND;

namespace mtgb
{
	class InputData;
	/// <summary>
	/// ウィンドウごとの入力機器リソース
	/// </summary>
	class InputResource : public WindowContextResource
	{
	  public:
		explicit InputResource(WindowContext _windowContext);

		void Update() override;
		void SetResource() override;
		void Release() override;
		/// <summary>
		/// このリソースが持っている入力機器の状態を返す
		/// </summary>
		/// <returns></returns>
		const InputData* GetInput()
		{
			return pInputData_;
		}

	  private:
		/// <summary>
		/// スティックの傾き状態を更新
		/// </summary>
		void UpdateStickTiltStates();
		void UpdateStickTiltState(InputData::StickTiltState& _state, StickType _stickType);
		/// <summary>
		/// 入力状態
		/// </summary>
		InputData* pInputData_;
		ComPtr<IDirectInputDevice8> pKeyDevice_;
		ComPtr<IDirectInputDevice8> pMouseDevice_;
		ComPtr<IDirectInputDevice8> pConrtollerDevice_;

		// コントローラーに割り当てられたGUID
		GUID assignedControllerGuid_;
		// コントローラーデバイスが初期化されたか
		bool isInitializedControllerDevice_;

		/// ImGui表示用 ///
		// 表示名
		std::string name_;
		// コントローラーの入力状態を表示する用のプロキシ
		ControllerProxy* pControllerProxy_;
		// マウスの入力状態を表示する用のプロキシ
		MouseStateProxy* pMouseStateProxy_;

		// コピーコンストラクタとコピー代入演算子を削除
		InputResource(const InputResource&)			   = delete;
		InputResource& operator=(const InputResource&) = delete;
	};
} // namespace mtgb