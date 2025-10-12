#include "Game.h"
#include "ISystem.h"
#include "Input.h"
#include "WindowResource.h"
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_win32.h"
#include "../ImGui/imgui_impl_dx11.h"
#include "WindowContextUtil.h"
#include "Direct3DResource.h"
#include "MTAssert.h"
#include "Screen.h"
using namespace mtgb;



/// <summary>
/// �E�B���h�E����̃��b�Z�[�W����M����ImGui�̓��͂�C�x���g��L���ɂ��邽�߂̃R�[���o�b�N�֐�
/// </summary>
/// <param name="hwnd">�E�B���h�E�n���h��</param>
/// <param name="msg">���b�Z�[�W</param>
/// <param name="wParam">�p�����[�^</param>
/// <param name="lParam">�p�����[�^</param>
/// <returns></returns>
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

/// <summary>
/// �e�E�B���h�E�ɋ��ʂ̏������L�q�B�����WNDCLASS�ɓn��
/// </summary>
/// <param name="hWnd"></param>
/// <param name="msg"></param>
/// <param name="wParam"></param>
/// <param name="lParam">�C���X�^���X���擾���邽�߂ɁACreateWindow��lpParam��this��n���K�v������</param>
/// <returns></returns>
LRESULT WindowResource::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	WindowResource* pThis;
	if (msg == WM_NCCREATE)
	{
		//�쐬���̃p�����[�^����this���擾�A�L���X�g
		pThis = static_cast<WindowResource*>(reinterpret_cast<LPCREATESTRUCT>(lParam)->lpCreateParams);
		//this��USERDATA��this��R�t����
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
		pThis->hWnd_ = hWnd;
	}
	else
	{
		//hWnd��this��R�Â��Ă������̂Ŏ擾
		pThis = reinterpret_cast<WindowResource*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		if (pThis)
		{
			pThis->hWnd_ = hWnd;
		}
	}
	if (pThis)
	{
		return pThis->HandleWindowMessage(hWnd, msg, wParam, lParam);
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);

}

LRESULT WindowResource::HandleWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
	}


	switch (msg)
	{
	case WM_CLOSE:
		Game::Exit();
		return 0;
	case WM_DESTROY:  // �E�B���h�E�����
		return S_OK;
	case WM_MOUSEMOVE:  // �}�E�X��������
		Game::System<Input>().UpdateMousePositionData(LOWORD(lParam), HIWORD(lParam));
		return S_OK;
	case WM_SIZE:  // �E�B���h�E�T�C�Y���ς����
	{
		if (this && wParam != SIZE_MINIMIZED)
		{
			if (!isInitialized_)
			{
				// �܂�����������Ă��Ȃ��Ȃ�X�L�b�v����
				return S_OK;
			}

			UINT width = LOWORD(lParam);
			UINT height = HIWORD(lParam);

			// �t���X�N���[���؂�ւ����̃��T�C�Y������L����
			//Game::System<WindowManager>().ResizeWindow(context_, width, height);
		}
		return S_OK;
	}

	//case WM_NCCALCSIZE:
	//	// wParam�ɂ��āFTRUE�Ȃ�NCCALCSIZE_PARAMS / FALSE�Ȃ�Rect*
	//	if (wParam) return 0;
	//	break;
	//case WM_NCHITTEST:
	//{
	//	LRESULT hitResult{ DefWindowProc(hWnd, msg, wParam, lParam) };
	//	switch (hitResult)
	//	{
	//	case HTLEFT:
	//	case HTRIGHT:
	//	case HTTOP:
	//	case HTBOTTOM:
	//	case HTTOPLEFT:
	//	case HTTOPRIGHT:
	//	case HTBOTTOMLEFT:
	//	case HTBOTTOMRIGHT:
	//		return HTCLIENT; // �T�C�Y�͕ς������Ȃ�
	//	default:
	//		return hitResult;
	//	}
	//}
	default:  // ����ȊO�̃��b�Z�[�W�͏��n
		break;
	}


	// NOTE: ���ꂪ�����Ă���ƃE�B���h�E�\������Ȃ����A�G���[�͏o�Ȃ����ŋ�J����(����)
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

HWND mtgb::WindowResource::GetHWND()
{
	return hWnd_;
}

void WindowResource::Initialize(WindowContext _windowContext)
{
	hWnd_ = Game::System<WindowManager>().CreateWindowContext(_windowContext);

	isActive_ = true;
	context_ = _windowContext;
	isInitialized_ = false;

	currInfo_.windowedStyle_ = GetWindowLong(hWnd_, GWL_STYLE);
	currInfo_.windowedExStyle_ = GetWindowLong(hWnd_, GWL_EXSTYLE);
	GetWindowRect(hWnd_, &currInfo_.windowedRect_);

	initialInfo_ = currInfo_;
}

void WindowResource::SetResource()
{

}

mtgb::WindowResource::WindowResource(const WindowResource& other)
	:WindowContextResource(other)
	,hWnd_(other.hWnd_)
	,isActive_(other.isActive_)
{

}

WindowResource* mtgb::WindowResource::Clone() const
{
	return new WindowResource(*this);
}

mtgb::WindowResource::WindowResource()
	: isInitialized_{false}
{
}

mtgb::WindowResource::~WindowResource()
{
	Release();
}

void mtgb::WindowResource::MarkInitialized()
{
	isInitialized_ = true;
}

void mtgb::WindowResource::OnResize(WindowContext _windowContext, UINT _width, UINT _height)
{
	if (_width == 0 || _height == 0) return;

	Game::System<Screen>().SetSize(static_cast<int>(_width), static_cast<int>(_height));

	//WindowConfig�̃T�C�Y���X�V
	WindowConfig config = Game::System<WindowManager>().GetWindowConfig(_windowContext);
	config.width = _width;
	config.height = _height;
	Game::System<WindowManager>().SetWindowConfig(_windowContext, config);

}

void mtgb::WindowResource::SetWindowMode()
{
	SetWindowModeImpl(currInfo_);
}


//void mtgb::WindowResource::SetInitialWindowMode()
//{
//	SetWindowModeImpl(initialInfo_);
//}

void mtgb::WindowResource::SetFullScreen(const RECT& _monitorRect)
{
	// �t���X�N���[���ɂȂ�
	isFullscreen_ = true;

	GetWindowInfo();

	// �E�B���h�E�X�^�C����g�Ȃ��|�b�v�A�b�v�ɕύX
	SetWindowLong(hWnd_, GWL_STYLE, currInfo_.windowedStyle_ & ~(WS_CAPTION | WS_THICKFRAME));
	SetWindowLong(hWnd_, GWL_EXSTYLE, currInfo_.windowedExStyle_ & ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE));

	// �w�肳�ꂽ���j�^�[���W�𒼐ڎg�p
	SetWindowPos(hWnd_, HWND_TOP,
		_monitorRect.left, _monitorRect.top,// �E�B���h�E�̈ʒu
		_monitorRect.right - _monitorRect.left,// �E�B���h�E�̃T�C�Y(��)
		_monitorRect.bottom - _monitorRect.top,// �E�B���h�E�̃T�C�Y(����)
		// �I�[�i�[(?)�E�B���h�E��Z�����͕ύX���Ȃ��A�X�^�C���̕ύX��K�p
		SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

}



void mtgb::WindowResource::GetWindowInfo()
{
	// ���݂̃E�B���h�E�̃X�^�C���ƈʒu��ۑ�
	currInfo_.windowedStyle_ = GetWindowLong(hWnd_, GWL_STYLE);
	currInfo_.windowedExStyle_ = GetWindowLong(hWnd_, GWL_EXSTYLE);
	GetWindowRect(hWnd_, &currInfo_.windowedRect_);
}

void mtgb::WindowResource::SetPosition(const RECT& _monitorRect)
{
	SetWindowPos(hWnd_, HWND_NOTOPMOST,
		_monitorRect.left,
		_monitorRect.top,
		_monitorRect.right - _monitorRect.left,
		_monitorRect.bottom - _monitorRect.top,
		// �I�[�i�[�E�B���h�E(?)��Z�����͕ύX���Ȃ��A�X�^�C���̕ύX��K�p
		SWP_NOOWNERZORDER);

	currInfo_.windowedRect_ = _monitorRect;
}

void mtgb::WindowResource::SetWindowModeImpl(WindowModeInfo _info)
{
	// �E�B���h�E���[�h�ɖ߂�
	isFullscreen_ = false;

	// �E�B���h�E�X�^�C�������ɖ߂�
	SetWindowLong(hWnd_, GWL_STYLE, _info.windowedStyle_);
	SetWindowLong(hWnd_, GWL_EXSTYLE, _info.windowedExStyle_);

	// �ۑ����Ă������E�B���h�E�̈ʒu�A�T�C�Y��߂�
	RECT rect = _info.windowedRect_;
	SetWindowPos(hWnd_, HWND_NOTOPMOST,
		rect.left,
		rect.top,
		rect.right - rect.left,
		rect.bottom - rect.top,
		// �I�[�i�[�E�B���h�E(?)��Z�����͕ύX���Ȃ��A�X�^�C���̕ύX��K�p
		SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
}

void mtgb::WindowResource::Release()
{
	DestroyWindow(hWnd_);
}


