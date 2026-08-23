#include "WindowManager.h"
#include "IncludingWindows.h"
#include "WindowContext/WindowContextResourceManager.h"
#include "WindowContext/WindowContext.h"
#include "WindowContext/WindowResource.h"
#include "WindowContext/WindowContextUtil.h"
#include "WindowContext/DXGIResource.h"
#include "MTAssert.h"
#include "Game.h"
#include "ISystem.h"
#include "MTStringUtility.h"
#include "ImGuiRenderer.h"
#include "Screen.h"

MSG* mtgb::WindowManager::pPeekedMessage_ { nullptr };
std::map<mtgb::WindowContext, mtgb::WindowConfig> mtgb::WindowManager::windowConfigMap_;

mtgb::WindowManager::WindowManager()
{
	pPeekedMessage_ = new MSG {};
}

mtgb::WindowManager::~WindowManager()
{
	delete pPeekedMessage_;
}

HWND mtgb::WindowManager::CreateWindowContext(WindowResource* _windowResource)
{
	WindowConfig config = WindowManager::GetWindowConfig(_windowResource->GetWindowContext());
	// ウィンドウ作成処理
	WNDCLASSEX windowClass {};
	windowClass.cbSize		  = sizeof(WNDCLASSEX);
	windowClass.hInstance	  = GetModuleHandle(NULL);
	windowClass.lpszClassName = config.className.c_str();
	windowClass.lpfnWndProc	  = mtgb::WindowResource::WndProc; // 仮のプロシージャ
	windowClass.style		  = CS_VREDRAW | CS_HREDRAW;
	windowClass.hIcon		  = LoadIcon(nullptr, IDI_APPLICATION);
	windowClass.hIconSm		  = LoadIcon(nullptr, IDI_WINLOGO);
	windowClass.hCursor		  = LoadCursor(nullptr, IDC_ARROW);
	windowClass.lpszMenuName  = nullptr;
	windowClass.cbClsExtra	  = 0;
	windowClass.cbWndExtra	  = 0;
	windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

	massert(RegisterClassEx(&windowClass) != 0 && "RegisterClassExに失敗 @WindowManager::CreateWindowContext");

	RECT windowRect { 0, 0, config.width, config.height };
	massert(
		AdjustWindowRectEx(&windowRect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_OVERLAPPEDWINDOW) != FALSE &&
		"AdjustWindowRectExに失敗 @WindowManager::CreateWindowContext"
	);

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
		reinterpret_cast<LPVOID>(_windowResource)
	);

	massert(hWnd != NULL && "ウィンドウの作成に失敗");
	massert(SetWindowText(hWnd, config.title.c_str()) && "SetWindowTextに失敗");

	return hWnd;
}

mtgb::Vector2Int mtgb::WindowManager::GetWindowSize(WindowContext _context)
{
	if (_context == WindowContext::BOTH)
	{
		return mtgb::Vector2Int { windowConfigMap_[WindowContext::FIRST].width,
								  windowConfigMap_[WindowContext::FIRST].height };
	}
	return mtgb::Vector2Int { windowConfigMap_[_context].width, windowConfigMap_[_context].height };
}

void mtgb::WindowManager::Initialize() {}

void mtgb::WindowManager::Update()
{
	if (PeekMessage(pPeekedMessage_, nullptr, 0u, 0u, PM_REMOVE))
	{
		TranslateMessage(pPeekedMessage_);
		DispatchMessage(pPeekedMessage_);
		return;
	}
}

void mtgb::WindowManager::Release() {}

void mtgb::WindowManager::SetWindowConfig(WindowContext _windowContext, const WindowConfig& _config)
{
	windowConfigMap_[_windowContext] = _config;
}

mtgb::WindowConfig mtgb::WindowManager::GetWindowConfig(WindowContext _windowContext)
{
	auto itr = windowConfigMap_.find(_windowContext);

	massert(itr != windowConfigMap_.end() && "指定されたWindowContextのConfigが見つかりません");
	return itr->second;
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
	mInfo.cbSize	  = sizeof(MONITORINFO);

	if (GetMonitorInfo(hMonitor, &mInfo))
	{
		RECT monitorRect = mInfo.rcMonitor;
		ChangeFullScreenState(_ctx, monitorRect);
	}
}

void mtgb::WindowManager::ChangeFullScreenState(WindowContext _ctx, const RECT& _rect)
{
	UINT winWidth, winHeight;

	WindowResource& winRes = WinCtxRes::Get<WindowResource>(_ctx);
	if (!winRes.IsFullScreen())
	{
		winRes.SetFullScreen(_rect);

		winWidth  = _rect.right - _rect.left;
		winHeight = _rect.bottom - _rect.top;
	}
	else
	{
		winRes.SetWindowMode();

		Vector2Int initialSize = Game::System<Screen>().GetInitialSize();
		winWidth			   = static_cast<UINT>(initialSize.x);
		winHeight			   = static_cast<UINT>(initialSize.y);
	}

	ResizeWindow(_ctx, winWidth, winHeight);
}

void mtgb::WindowManager::ResizeWindow(WindowContext _windowContext, UINT _width, UINT _height)
{
	// パイプラインにバインドした設定、スワップチェーンのバックバッファを参照するリソースのリセット
	Game::System<DirectX11Manager>().ClearState();
	if (_windowContext == WindowContext::FIRST)
	{
		Game::System<ImGuiRenderer>().ResetComPtrs();
	}
	// リソースを更新
	Game::System<WindowContextResourceManager>().OnResizeAll(_windowContext, _width, _height);

	// デフォルトの設定をセット
	Game::System<DirectX11Manager>().SetDefaultStates();

	// ImGuiも更新
	if (_windowContext == WindowContext::FIRST)
	{
		Game::System<ImGuiRenderer>().OnResize(_width, _height);
	}
}