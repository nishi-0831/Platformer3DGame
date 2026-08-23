#include "SingleWindow.h"
#include "WindowContext/WindowContext.h"
#include "WindowManager.h"
#include "string"
#include "Game.h"
#include "Screen.h"
#include "DirectX11Manager.h"
#include "DirectX11Draw.h"
#include "WindowContext/WindowContextResourceManager.h"
#include "WindowContext/WindowContextUtil.h"
#include "WindowContext/WindowResource.h"
mtgb::SingleWindow::SingleWindow()
{
	context_ = WindowContext::FIRST;
}

mtgb::SingleWindow::~SingleWindow() {}

void mtgb::SingleWindow::Initialize()
{
	const Vector2Int SCREEN_SIZE { Game::System<Screen>().GetSize() };

	WindowConfig config1 = { .title		= "Game",
							 .className = "GameWindowClass",
							 .width		= SCREEN_SIZE.x,
							 .height	= SCREEN_SIZE.y,
							 .x			= 0,
							 .y			= 0 };

	// リソースの初期化もここで行う
	Game::System<WindowManager>().SetWindowConfig(WindowContext::FIRST, config1);

	Game::System<WindowContextResourceManager>().CreateResource(WindowContext::FIRST);

	HWND hWnd1 = WinCtxRes::GetHWND(WindowContext::FIRST);

	ShowWindow(hWnd1, SW_SHOW);

	// ウィンドウの表示、初期化完了をマーク
	Game::System<WindowContextResourceManager>().Get<WindowResource>(WindowContext::FIRST).MarkInitialized();

	Game::System<WindowContextResourceManager>().ChangeActiveResource(WindowContext::FIRST);

	SetForegroundWindow(hWnd1);
}

void mtgb::SingleWindow::Update() {}