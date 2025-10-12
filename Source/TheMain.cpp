#include "IncludingWindows.h"
#include <mtgb.h>
#include "SampleGame.h"
namespace mtgb
{
#ifdef _DEBUG   // �f�o�b�N���ɂ̂ݎ��s
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

	// ���������[�N�ӏ����o�͂���}�N��
#define new new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#endif
}
// �G���g���|�C���g
int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_  HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd)
{
	_CrtSetBreakAlloc(125);
	mtgb::Game::Run<SampleGame>();
	_CrtDumpMemoryLeaks();
	return 0x00;
}
