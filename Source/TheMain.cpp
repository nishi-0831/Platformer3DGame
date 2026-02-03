#include "IncludingWindows.h"
#include <mtgb.h>
#include "SampleGame.h"
namespace mtgb
{
#ifdef _DEBUG // デバック時にのみ実行
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

	// メモリリーク箇所を出力するマクロ
#define new new (_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
} // namespace mtgb
// エントリポイント
int WINAPI WinMain(_In_ HINSTANCE _hInstance, _In_opt_ HINSTANCE _hPrevInstance, _In_ LPSTR _lpCmdLine, _In_ int _nShowCmd)
{
	_CrtSetBreakAlloc(125);
	mtgb::Game::Run<SampleGame>();
	_CrtDumpMemoryLeaks();
	return 0x00;
}
