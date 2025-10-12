#pragma once
#include "WindowContextResource.h"
#include "Game.h"
#include "ISystem.h"
#include "DirectX11Manager.h"
#include "MTAssert.h"
#include "WindowContext.h"
#include "WindowManager.h"

#include "includingWindows.h"

#include <map>
#include <typeindex>

namespace mtgb
{
	// �E�B���h�E���[�h���̏��
	struct WindowModeInfo
	{
		RECT windowedRect_;		//�ʒu�ƃT�C�Y
		LONG windowedStyle_;	// �X�^�C��
		LONG windowedExStyle_;	// �g���X�^�C��
	};
	class WindowResource : public WindowContextResource
	{
	public:
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		LRESULT HandleWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		HWND GetHWND();

		void Initialize(WindowContext _windowContext) override;
		void SetResource() override;
		
		WindowResource(const WindowResource& other);
		// WindowContextResource ����Čp������܂���
		WindowResource* Clone() const override;
		WindowResource();
		~WindowResource();

		/// <summary>
		/// <para> ���\�[�X�̏����������������Ƃ����t���O�� true�ɂ��� </para>
		/// </summary>
		void MarkInitialized();
		void OnResize(WindowContext _windowContext, UINT _width, UINT _height) override;

		/// <summary>
		/// ���O�̃E�B���h�E���[�h�ɖ߂�
		/// </summary>
		void SetWindowMode();
		/// <summary>
		/// �����̃E�B���h�E���[�h�ɖ߂�
		/// </summary>
		//void SetInitialWindowMode();

		void SetFullScreen(const RECT& _monitorRect);

		
		/// <summary>
		/// �t���X�N���[���ɂ���O�ɏ���ێ����Ă���
		/// </summary>
		void GetWindowInfo();

		bool IsFullScreen() const { return isFullscreen_; }
		void SetPosition(const RECT& _monitorRect);
	private:
		void SetWindowModeImpl(WindowModeInfo _info);
		WindowContext context_;
		HWND hWnd_;
		bool isActive_;
		bool isFullscreen_; // �t���X�N���[�����ۂ�

		WindowModeInfo currInfo_; // ���݂̃E�B���h�E���[�h���̏��
		WindowModeInfo initialInfo_; // �����̃E�B���h�E���[�h���̏��


		// ������������������
		bool isInitialized_;

		// WindowContextResource ����Čp������܂���
		void Release() override;

	};
}
