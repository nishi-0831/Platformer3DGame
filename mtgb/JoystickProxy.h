#pragma once
#include "IncludingInput.h"
#include "ImGuiShowable.h"
#include <string>
namespace mtgb
{
	/// <summary>
	/// �W���C�X�e�B�b�N�̓��͂�ImGui�ŕ\������p
	/// </summary>
	struct JoystickProxy 
	{
		JoystickProxy(const DIJOYSTATE& js);
		LONG lX;
		LONG lY;
		LONG lZ;
		LONG lRx;// ���̃g���K�[�{�^��
		LONG lRy;// �E�̃g���K�[�{�^��
		LONG lRz;
		LONG rglSlider[2];
		DWORD rgdwPOV[4];
		BYTE rgbButtons[32];

		std::string connectionStatus;
		std::string assignmentStatus;
		std::string lastErrorMessage;
		std::string deviceName;
		std::string deviceProductName;
		bool isConnected;
		bool isAssigned;

		void UpdateFromInput(GUID guid);
		void UpdateInputData(const DIJOYSTATE& js);
		//JoystickProxy operator=(const DIJOYSTATE& js);
	};

	// Dual Shock�̏ꍇ
	// lRx,lRy�����A�E�̃g���K�[�{�^��
	// lY�͏�ɓ|���ƕ��A���Ő��AlX�͍������A�E����
}