#include "Game.h"
#include "ISystem.h"
#include "Input/Input.h"
#include "WindowResource.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"
#include "WindowContextUtil.h"
#include "Direct3DResource.h"
#include "MTAssert.h"
#include "Screen.h"
#include "WindowManager.h"

using namespace mtgb;

/// <summary>
/// ウィンドウからのメッセージを受信してImGuiの入力やイベントを有効にするためのコールバック関数
/// </summary>
/// <param name="hwnd">ウィンドウハンドル</param>
/// <param name="msg">メッセージ</param>
/// <param name="wParam">パラメータ</param>
/// <param name="lParam">パラメータ</param>
/// <returns></returns>
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND _hwnd, UINT _msg, WPARAM _wParam, LPARAM _lParam);

/// <summary>
/// 各ウィンドウに共通の処理を記述。これをWNDCLASSに渡す
/// </summary>
/// <param name="hWnd"></param>
/// <param name="msg"></param>
/// <param name="wParam"></param>
/// <param name="lParam">インスタンスを取得するために、CreateWindowのlpParamにthisを渡す必要がある</param>
/// <returns></returns>
LRESULT WindowResource::WndProc(HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam)
{
	WindowResource* pThis;
	if (_msg == WM_NCCREATE)
	{
		// 作成時のパラメータからthisを取得、キャスト
		pThis = static_cast<WindowResource*>(reinterpret_cast<LPCREATESTRUCT>(_lParam)->lpCreateParams);
		// thisのUSERDATAにthisを紐付ける
		SetWindowLongPtr(_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
		pThis->hWnd_ = _hWnd;
	}
	else
	{
		// hWndにthisを紐づけておいたので取得
		pThis = reinterpret_cast<WindowResource*>(GetWindowLongPtr(_hWnd, GWLP_USERDATA));
		if (pThis)
		{
			pThis->hWnd_ = _hWnd;
		}
	}
	if (pThis)
	{
		return pThis->HandleWindowMessage(_hWnd, _msg, _wParam, _lParam);
	}
	return DefWindowProc(_hWnd, _msg, _wParam, _lParam);
}

LRESULT WindowResource::HandleWindowMessage(HWND _hWnd, UINT _msg, WPARAM _wParam, LPARAM _lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(_hWnd, _msg, _wParam, _lParam))
	{
		return true;
	}

	switch (_msg)
	{
		case WM_CLOSE :
			Game::Exit();
			return 0;
		case WM_DESTROY : // ウィンドウを閉じた
			return S_OK;
		case WM_MOUSEMOVE : // マウスが動いた
			Game::System<Input>().UpdateMousePositionData(LOWORD(_lParam), HIWORD(_lParam));
			return S_OK;
		case WM_SIZE : // ウィンドウサイズが変わった
		{
			return S_OK;
		}

		default : // それ以外のメッセージは譲渡
			break;
	}

	return DefWindowProc(_hWnd, _msg, _wParam, _lParam);
}

HWND mtgb::WindowResource::GetHWND()
{
	return hWnd_;
}

mtgb::WindowResource::WindowResource(WindowContext _windowContext)
	: WindowContextResource(_windowContext)
	, hWnd_ { nullptr }
	, isActive_ { false }
	, isFullscreen_ { false }
	, currInfo_ {}
	, initialInfo_ {}
	, isInitialized_ { false }
{
	hWnd_		   = Game::System<WindowManager>().CreateWindowContext(this);
	isActive_	   = true;
	isInitialized_ = false;

	currInfo_.windowedStyle_   = GetWindowLong(hWnd_, GWL_STYLE);
	currInfo_.windowedExStyle_ = GetWindowLong(hWnd_, GWL_EXSTYLE);
	GetWindowRect(hWnd_, &currInfo_.windowedRect_);

	initialInfo_ = currInfo_;
}

void WindowResource::SetResource() {}

mtgb::WindowResource::~WindowResource()
{
	DestroyWindow(hWnd_);
}

void mtgb::WindowResource::MarkInitialized()
{
	isInitialized_ = true;
}

void mtgb::WindowResource::OnResize(UINT _width, UINT _height)
{
	if (_width == 0 || _height == 0)
		return;

	Game::System<Screen>().SetSize(static_cast<int>(_width), static_cast<int>(_height));

	// WindowConfigのサイズを更新
	WindowConfig config = Game::System<WindowManager>().GetWindowConfig(windowContext_);
	config.width		= _width;
	config.height		= _height;
	Game::System<WindowManager>().SetWindowConfig(windowContext_, config);
}

void mtgb::WindowResource::SetWindowMode()
{
	SetWindowModeImpl(currInfo_);
}

void mtgb::WindowResource::SetFullScreen(const RECT& _monitorRect)
{
	// フルスクリーンになる
	isFullscreen_ = true;

	GetWindowInfo();

	// ウィンドウスタイルを枠なしポップアップに変更
	SetWindowLong(hWnd_, GWL_STYLE, currInfo_.windowedStyle_ & ~(WS_CAPTION | WS_THICKFRAME));
	SetWindowLong(
		hWnd_,
		GWL_EXSTYLE,
		currInfo_.windowedExStyle_ & ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE)
	);

	// 指定されたモニター座標を直接使用
	SetWindowPos(
		hWnd_,
		HWND_TOP,
		_monitorRect.left,
		_monitorRect.top,						// ウィンドウの位置
		_monitorRect.right - _monitorRect.left, // ウィンドウのサイズ(幅)
		_monitorRect.bottom - _monitorRect.top, // ウィンドウのサイズ(高さ)
		// オーナー(?)ウィンドウのZ順序は変更しない、スタイルの変更を適用
		SWP_NOOWNERZORDER | SWP_FRAMECHANGED
	);
}

void mtgb::WindowResource::GetWindowInfo()
{
	// 現在のウィンドウのスタイルと位置を保存
	currInfo_.windowedStyle_   = GetWindowLong(hWnd_, GWL_STYLE);
	currInfo_.windowedExStyle_ = GetWindowLong(hWnd_, GWL_EXSTYLE);
	GetWindowRect(hWnd_, &currInfo_.windowedRect_);
}

void mtgb::WindowResource::SetPosition(const RECT& _monitorRect)
{
	SetWindowPos(
		hWnd_,
		HWND_NOTOPMOST,
		_monitorRect.left,
		_monitorRect.top,
		_monitorRect.right - _monitorRect.left,
		_monitorRect.bottom - _monitorRect.top,
		// オーナーウィンドウ(?)のZ順序は変更しない、スタイルの変更を適用
		SWP_NOOWNERZORDER
	);

	currInfo_.windowedRect_ = _monitorRect;
}

void mtgb::WindowResource::SetWindowModeImpl(const WindowModeInfo& _info)
{
	// ウィンドウモードに戻る
	isFullscreen_ = false;

	// ウィンドウスタイルを元に戻す
	SetWindowLong(hWnd_, GWL_STYLE, _info.windowedStyle_);
	SetWindowLong(hWnd_, GWL_EXSTYLE, _info.windowedExStyle_);

	// 保存しておいたウィンドウの位置、サイズを戻す
	RECT rect = _info.windowedRect_;
	SetWindowPos(
		hWnd_,
		HWND_NOTOPMOST,
		rect.left,
		rect.top,
		rect.right - rect.left,
		rect.bottom - rect.top,
		// オーナーウィンドウ(?)のZ順序は変更しない、スタイルの変更を適用
		SWP_NOOWNERZORDER | SWP_FRAMECHANGED
	);
}

void mtgb::WindowResource::Release()
{
	DestroyWindow(hWnd_);
}

mtgb::WindowModeInfo::WindowModeInfo()
	: windowedRect_ { 0, 0, 0, 0 }
	, windowedStyle_ { 0 }
	, windowedExStyle_ { 0 }
{
}
