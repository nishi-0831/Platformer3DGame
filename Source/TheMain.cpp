#include "IncludingWindows.h"
#include <mtgb.h>
#include "SampleGame.h"
// エントリポイント
int WINAPI
WinMain(_In_ HINSTANCE _hInstance, _In_opt_ HINSTANCE _hPrevInstance, _In_ LPSTR _lpCmdLine, _In_ int _nShowCmd)
{
	_CrtSetBreakAlloc(125);
	mtgb::Game::Run<SampleGame>();
	return 0x00;
}
