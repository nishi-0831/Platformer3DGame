#include "WindowManager.h"
#include "WindowContextResourceManager.h"
#include "WindowContext.h"
//#include <Windows.h>
//#include "WindowRenderContext.h"
#include "IncludingWindows.h"
#include "MTAssert.h"
#include "WindowResource.h"
#include "Game.h"
#include "ISystem.h"
#include "MTStringUtility.h"
#include "ImGuiRenderer.h"
#include "WindowContextUtil.h"
#include "DXGIResource.h"
#include "Screen.h"

MSG* mtgb::WindowManager::pPeekedMessage_{ nullptr };
std::map<mtgb::WindowContext, mtgb::WindowConfig> mtgb::WindowManager::windowConfigMap_;

mtgb::WindowManager::WindowManager() 
{
	pPeekedMessage_ = new MSG{};
}

mtgb::WindowManager::~WindowManager()
{
	delete pPeekedMessage_;
}

HWND mtgb::WindowManager::CreateWindowContext(WindowContext context)
{
	WindowConfig config = WindowManager::GetWindowConfig(context);
	// �E�B���h�E�쐬����
	WNDCLASSEX windowClass{};
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.hInstance = GetModuleHandle(NULL);
	windowClass.lpszClassName = config.className.c_str();
	windowClass.lpfnWndProc = mtgb::WindowResource::WndProc; // ���̃v���V�[�W��
	windowClass.style = CS_VREDRAW | CS_HREDRAW;
	windowClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	windowClass.hIconSm = LoadIcon(nullptr, IDI_WINLOGO);
	windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	windowClass.lpszMenuName = nullptr;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

	massert(RegisterClassEx(&windowClass) != 0
		&& "RegisterClassEx�Ɏ��s @WindowManager::CreateWindowContext");

	RECT windowRect{ 0, 0, config.width, config.height };
	massert(
		AdjustWindowRectEx(
			&windowRect,
			WS_OVERLAPPEDWINDOW,
			FALSE,
			WS_EX_OVERLAPPEDWINDOW) != FALSE
		&& "AdjustWindowRectEx�Ɏ��s @WindowManager::CreateWindowContext");

	WindowResource& windowResource = WindowManager::GetWindowResource(context);

	HWND hWnd = CreateWindowEx(
		0,
		config.className.c_str(),
		config.title.c_str(),
		WS_OVERLAPPEDWINDOW,
		config.x,
		config.y,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr,
		nullptr,
		GetModuleHandle(NULL),
		reinterpret_cast<LPVOID>(&windowResource)
	);

	massert(hWnd != NULL
		&& "�E�B���h�E�̍쐬�Ɏ��s");
	massert(SetWindowText(hWnd, config.title.c_str())
		&& "SetWindowText�Ɏ��s");

	
	return hWnd;
}

mtgb::Vector2Int mtgb::WindowManager::GetWindowSize(WindowContext context)
{
	if (context == WindowContext::Both)
	{
		return mtgb::Vector2Int{ windowConfigMap_[WindowContext::First].width,windowConfigMap_[WindowContext::First].height };
	}
	return mtgb::Vector2Int{ windowConfigMap_[context].width,windowConfigMap_[context].height };
}

void mtgb::WindowManager::Initialize()
{
}

void mtgb::WindowManager::Update()
{
	if (PeekMessage(
		pPeekedMessage_,
		nullptr,
		0u,
		0u,
		PM_REMOVE))
	{
		TranslateMessage(pPeekedMessage_);
		DispatchMessage(pPeekedMessage_);
		return;
	}
}

void mtgb::WindowManager::Release()
{
}

void mtgb::WindowManager::SetWindowConfig(WindowContext windowContext, const WindowConfig& config)
{
	windowConfigMap_[windowContext] = config;
}

mtgb::WindowConfig mtgb::WindowManager::GetWindowConfig(WindowContext windowContext)
{
	auto itr = windowConfigMap_.find(windowContext);
	
	massert(itr != windowConfigMap_.end() && "�w�肳�ꂽWindowContext��Config��������܂���");
	return itr->second;
}



mtgb::WindowResource& mtgb::WindowManager::GetWindowResource(WindowContext windowContext)
{
	return Game::System<WindowContextResourceManager>().Get<WindowResource>(windowContext);
}

void mtgb::WindowManager::ChangeFullScreenState(WindowContext _ctx)
{
	ChangeFullScreenState(_ctx, WinCtxRes::Get<DXGIResource>(_ctx).GetAssignedMonitorRect());
}

void mtgb::WindowManager::ChangeFullScreenStateNearestMonitor(WindowContext _ctx)
{
	WindowResource& winRes = WinCtxRes::Get<WindowResource>(_ctx);

	HMONITOR hMonitor = MonitorFromWindow(winRes.GetHWND(), MONITOR_DEFAULTTONEAREST);
	MONITORINFO mInfo = {};
	mInfo.cbSize = sizeof(MONITORINFO);

	if (GetMonitorInfo(hMonitor, &mInfo))
	{
		RECT monitorRect = mInfo.rcMonitor;
		ChangeFullScreenState(_ctx,monitorRect);
	}
}

void mtgb::WindowManager::ChangeFullScreenState(WindowContext _ctx, const RECT& _rect)
{
	UINT winWidth, winHeight;

	WindowResource& winRes = WinCtxRes::Get<WindowResource>(_ctx);
	if (!winRes.IsFullScreen())
	{
		winRes.SetFullScreen(_rect);

		winWidth = _rect.right - _rect.left;
		winHeight = _rect.bottom - _rect.top;
	}
	else
	{
		winRes.SetWindowMode();

		Vector2Int initialSize = Game::System<Screen>().GetInitialSize();
		winWidth = static_cast<UINT>(initialSize.x);
		winHeight = static_cast<UINT>(initialSize.y);
	}

	ResizeWindow(_ctx, winWidth, winHeight);
}

void mtgb::WindowManager::ResizeWindow(WindowContext _windowContext, UINT _width, UINT _height)
{
	// �p�C�v���C���Ƀo�C���h�����ݒ�A�X���b�v�`�F�[���̃o�b�N�o�b�t�@���Q�Ƃ��郊�\�[�X�̃��Z�b�g
	Game::System<DirectX11Manager>().ClearState();
	if (_windowContext == WindowContext::First)
	{
		Game::System<ImGuiRenderer>().ResetComPtrs();
	}
	// ���\�[�X���X�V
	Game::System<WindowContextResourceManager>().OnResizeAll(_windowContext, _width, _height);

	// �f�t�H���g�̐ݒ���Z�b�g
	Game::System<DirectX11Manager>().SetDefaultStates();

	// ImGui���X�V
	if (_windowContext == WindowContext::First)
	{
		Game::System<ImGuiRenderer>().OnResize(_width,_height);
	}
}

#pragma region SwapWindowPos()
//void mtgb::WindowManager::SwapWindowPos(WindowContext _ctx1, WindowContext _ctx2)
//{
//	DXGIResource& dxgiRes1 = WinCtxRes::Get<DXGIResource>(_ctx1);
//	DXGIResource& dxgiRes2 = WinCtxRes::Get<DXGIResource>(_ctx2);
//
//	RECT monitorRect1 = dxgiRes1.GetAssignedMonitorRect();
//	RECT monitorRect2 = dxgiRes2.GetAssignedMonitorRect();
//
//	WindowResource& winRes1 = WinCtxRes::Get<WindowResource>(_ctx1);
//	WindowResource& winRes2 = WinCtxRes::Get<WindowResource>(_ctx2);
//
//	bool win1IsFullScreen = winRes1.IsFullScreen();
//	bool win2IsFullScreen = winRes2.IsFullScreen();
//	
//	
//	 
//	// �ǂ�����t���X�N���[��
//	if (win1IsFullScreen == true && win2IsFullScreen == true)
//	{
//		UINT win1Width = monitorRect1.right - monitorRect1.left;
//		UINT win2Width = monitorRect2.right - monitorRect2.left;
//
//		UINT win1Height = monitorRect1.bottom - monitorRect1.top;
//		UINT win2Height = monitorRect2.bottom - monitorRect2.top;
//
//		// �X�N���[���̃T�C�Y���Ⴄ��
//		if (win1Width != win2Width || win1Height != win2Height)
//		{
//			// �T�C�Y���ύX
//			winRes1.SetFullScreen(monitorRect2);
//			winRes2.SetFullScreen(monitorRect1);
//		}
//		else
//		{
//			// �ʒu�����ύX
//			winRes1.SetPosition(monitorRect2);
//			winRes2.SetPosition(monitorRect1);
//		}
//	}
//	// �ǂ�����E�B���h�E���[�h
//	else if (win1IsFullScreen == false && win2IsFullScreen == false)
//	{
//		// �ʒu�����ύX
//		winRes1.SetPosition(monitorRect2);
//		winRes2.SetPosition(monitorRect1);
//	}
//	// �ǂ��炩���t���X�N���[���A�ǂ��炩���E�B���h�E���[�h
//	else
//	{
//		ChangeFullScreenState(_ctx1, monitorRect2);
//		ChangeFullScreenState(_ctx2, monitorRect1);
//	}
//}
#pragma endregion