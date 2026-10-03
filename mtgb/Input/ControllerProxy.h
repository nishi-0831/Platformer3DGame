#pragma once
#include "IncludingInput.h"
#include "Editor/ImGuiShowable.h"
#include <string>
namespace mtgb
{
	/// <summary>
	/// コントローラーの入力をImGuiで表示する用
	/// </summary>
	struct ControllerProxy
	{
		ControllerProxy(const DIJOYSTATE& _js);
		LONG lX;
		LONG lY;
		LONG lZ;
		LONG lRx; // 左のトリガーボタン
		LONG lRy; // 右のトリガーボタン
		LONG lRz;
		LONG rglSlider[2];
		DWORD rgdwPOV[4];
		BYTE rgbButtons[32];

		std::string lastErrorMessage;
		std::string deviceName;
		std::string deviceProductName;

		void UpdateFromInput(GUID _guid);
		void UpdateInputData(const DIJOYSTATE& _js);
	};
} // namespace mtgb