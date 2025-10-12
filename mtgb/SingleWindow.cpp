#include "SingleWindow.h"
#include "WindowContext.h"
#include "WindowManager.h"
#include "string"
#include "Game.h"
#include "Screen.h"
#include "DirectX11Manager.h"
#include "DirectX11Draw.h"
#include "WindowContextResourceManager.h"
#include "WindowContextUtil.h"
#include "WindowResource.h"
mtgb::SingleWindow::SingleWindow()
{
	context_ = WindowContext::First;
}

mtgb::SingleWindow::~SingleWindow()
{
}

void mtgb::SingleWindow::Initialize()
{
	const Vector2Int SCREEN_SIZE{ Game::System<Screen>().GetSize() };

	WindowConfig config1 =
	{
		.title = "Player1",
		.className = "Player1WindowClass",
		.width = SCREEN_SIZE.x,
		.height = SCREEN_SIZE.y,
		.x = 0,
		.y = 0
	};

	//リソースの初期化もここで行う
	Game::System<WindowManager>().SetWindowConfig(WindowContext::First, config1);


	Game::System<WindowContextResourceManager>().CreateResource(WindowContext::First);

	HWND hWnd1 = WinCtxRes::GetHWND(WindowContext::First);

	ShowWindow(hWnd1, SW_SHOW);

	// ウィンドウの表示、初期化完了をマーク
	Game::System<WindowContextResourceManager>().Get<WindowResource>(WindowContext::First).MarkInitialized();

	Game::System<WindowContextResourceManager>().ChangeActiveResource(WindowContext::First);

	SetForegroundWindow(hWnd1);
}

void mtgb::SingleWindow::Update()
{
}