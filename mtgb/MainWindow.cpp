#include "MainWindow.h"
#include "IncludingWindows.h"
#include "Game.h"
#include "MTAssert.h"
#include "Screen.h"
#include "Input.h"
#include "ImGuiRenderer.h"
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_win32.h"
#include "../ImGui/imgui_impl_dx11.h"

#define IMGUI 1

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
/// �E�B���h�E����̃��b�Z�[�W����M����R�[���o�b�N�֐�
/// </summary>
/// <param name="hWnd">�E�B���h�E�n���h��</param>
/// <param name="message">���b�Z�[�W</param>
/// <param name="wParam">�p�����[�^</param>
/// <param name="lParam">�p�����[�^</param>
/// <returns>����</returns>
static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);




mtgb::MainWindow::MainWindow() :
	hWnd_{ nullptr },
	pPeekedMessage_{ nullptr }
{
	pPeekedMessage_ = new MSG{};
}

mtgb::MainWindow::~MainWindow()
{
	delete pPeekedMessage_;
}

void mtgb::MainWindow::Initialize()
{
	WNDCLASSEX windowClass{};  // �E�B���h�E�̐ݒ�
	GenerateWndClassEx(&windowClass);

	massert(RegisterClassEx(&windowClass) != 0  // �E�B���h�E�N���X�̓o�^�ɐ���
		&& "�E�B���h�E�N���X�̓o�^�Ɏ��s");

	const Vector2Int SCREEN_SIZE{ Game::System<Screen>().GetSize() };

	RECT windowRect{ 0, 0, SCREEN_SIZE.x, SCREEN_SIZE.y };
	massert(
		AdjustWindowRectEx(
			&windowRect,
			WS_OVERLAPPEDWINDOW,
			FALSE,
			WS_EX_OVERLAPPEDWINDOW) != FALSE  // �X�N���[���{�[�_���l�������E�B���h�E�T�C�Y�̎擾�ɐ���
		&& "�X�N���[���{�[�_���l�������E�B���h�E�T�C�Y�̎擾�Ɏ��s");

	{  // window�̍쐬
		DWORD     exWindowStyle{ WS_EX_OVERLAPPEDWINDOW };
		LPCSTR    className{ Game::Title().data() };
		LPCSTR    windowName{ Game::Title().data() };
		DWORD     windowStyle{ WS_OVERLAPPEDWINDOW };//& ~WS_THICKFRAME };
		int       windowPositionX{ CW_USEDEFAULT };
		int       windowPositionY{ CW_USEDEFAULT };
		int       windowWidth{ windowRect.right - windowRect.left };
		int       windowHeight{ windowRect.bottom - windowRect.top };
		HWND      hWndParent{ nullptr };
		HMENU     hMenu{ nullptr };
		HINSTANCE hInstance{ GetModuleHandle(NULL) };
		massert(hInstance != NULL  // ���W���[���n���h���̎擾�ɐ���
			&& "���W���[���n���h���̎擾�Ɏ��s");
		LPVOID    param{ nullptr };

		hWnd_ = CreateWindowEx(
			exWindowStyle,
			className,
			windowName,
			windowStyle,
			windowPositionX,
			windowPositionY,
			windowWidth,
			windowHeight,
			hWndParent,
			hMenu,
			hInstance,
			param);
	}

	massert(hWnd_ != NULL  // �E�B���h�E�̍쐬�ɐ������Ă���
		&& "�E�B���h�E�̍쐬�Ɏ��s");

	massert(IsWindow(hWnd_)  // �E�B���h�E�n���h�����������쐬����Ă���
		&& "Window�ł͂Ȃ��n���h��������Ă��܂���");

	// NOTE: ShowWindow�̖߂�l�ɒ���
	//  REF: https://learn.microsoft.com/ja-jp/windows/win32/api/winuser/nf-winuser-showwindow
	ShowWindow(hWnd_, SW_SHOWDEFAULT);  // �E�B���h�E��\��
}

void mtgb::MainWindow::Update()
{
	// MEMO: ��2������ nullptr ���w�肵�Ă��邽�߁A
	//     : ���C���E�B���h�E�Ɍ��炸�A�X���b�h�ɑ�����S�Ă�MSG�擾
	//  REF: https://learn.microsoft.com/ja-jp/windows/win32/api/winuser/nf-winuser-peekmessagea
	if (PeekMessage(
		pPeekedMessage_,
		nullptr,
		0u,  // �t�B���^���� (�ŏ��̃��b�Z�[�W)
		0u,  // �t�B���^���� (�Ō�̃��b�Z�[�W)
		PM_REMOVE))  // �s�[�N�������Ƃ̑S�ăf�L���[
	{
		TranslateMessage(pPeekedMessage_);  // ���z�L�[���͂𕶎����̓C�x���g�Ƃ���enqueue
		DispatchMessage(pPeekedMessage_);  // enqueue�������b�Z�[�W�����ۂɏ���������
		return;
	}
}

void mtgb::MainWindow::GenerateWndClassEx(WNDCLASSEX* _pWndClassEx)
{
	_pWndClassEx->cbSize = sizeof(WNDCLASSEX);                 // �\���̂̃T�C�Y
	_pWndClassEx->hInstance = GetModuleHandle(NULL);           // �C���X�^���X�n���h��
	_pWndClassEx->lpszClassName = Game::Title().data();        // �E�B���h�E�N���X��
	_pWndClassEx->lpfnWndProc = WndProc;                       // ���b�Z�[�W���󂯎��R�[���o�b�N�֐�
	_pWndClassEx->style = CS_VREDRAW | CS_HREDRAW;             // �X�^�C��
	_pWndClassEx->hIcon = LoadIcon(nullptr, IDI_APPLICATION);  // �A�C�R��
	_pWndClassEx->hIconSm = LoadIcon(nullptr, IDI_WINLOGO);    // �������A�C�R��
	_pWndClassEx->hCursor = LoadCursor(nullptr, IDC_ARROW);    // �}�E�X�J�[�\��
	_pWndClassEx->lpszMenuName = nullptr;                      // ���j���[
	_pWndClassEx->cbClsExtra = 0;
	_pWndClassEx->cbWndExtra = 0;
	_pWndClassEx->hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);  // �w�i�F ��
}



static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
	{
		return true;
	}
	switch (message)
	{
	case WM_DESTROY:  // �E�B���h�E�����
		Game::Exit();
		return S_OK;
	case WM_MOUSEMOVE:  // �}�E�X��������
		Game::System<Input>().UpdateMousePositionData(LOWORD(lParam), HIWORD(lParam));
		return S_OK;
	case WM_SIZE:  // �E�B���h�E�T�C�Y���ς����
		
		return S_OK;
	case WM_NCCALCSIZE:
		// wParam�ɂ��āFTRUE�Ȃ�NCCALCSIZE_PARAMS / FALSE�Ȃ�Rect*
		if (wParam) return 0;
		break;
	case WM_NCHITTEST:
	{
		LRESULT hitResult{ DefWindowProc(hWnd, message, wParam, lParam) };
		switch (hitResult)
		{
		case HTLEFT:
		case HTRIGHT:
		case HTTOP:
		case HTBOTTOM:
		case HTTOPLEFT:
		case HTTOPRIGHT:
		case HTBOTTOMLEFT:
		case HTBOTTOMRIGHT:
			return HTCLIENT; // �T�C�Y�͕ς������Ȃ�
		default:
			return hitResult;
		}
	}
	default:  // ����ȊO�̃��b�Z�[�W�͏��n
		break;
	}


	// NOTE: ���ꂪ�����Ă���ƃE�B���h�E�\������Ȃ����A�G���[�͏o�Ȃ����ŋ�J����(����)
	return DefWindowProc(hWnd, message, wParam, lParam);
}
