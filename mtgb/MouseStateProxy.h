#pragma once
#include "IncludingInput.h"
#include "ImGuiShowable.h"

namespace mtgb
{
	/// <summary>
	/// �}�E�X�̓��͂�ImGui�ŕ\������p
	/// </summary>
	struct MouseStateProxy
	{
		MouseStateProxy(const _DIMOUSESTATE& _ms);
		LONG lX;
		LONG lY;
		LONG lZ;
		BYTE rgbButtons[4];

		void UpdateInputData(const _DIMOUSESTATE& _ms);
	};
}