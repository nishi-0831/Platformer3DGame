#pragma once
#include "ISystem.h"

typedef struct tagWNDCLASSEXA WNDCLASSEX;
typedef struct HWND__* HWND;
typedef struct tagMSG MSG;

namespace mtgb
{
	class MainWindow : public ISystem
	{
	public:
		MainWindow();
		~MainWindow();

		void Initialize() override;
		void Update() override;

		/// <summary>
		/// ���C���E�B���h�E�̃n���h�����擾
		/// </summary>
		const HWND GetHWND() const { return hWnd_; }

	private:
		void GenerateWndClassEx(WNDCLASSEX* _pWndClassEx);

	private:
		HWND hWnd_;
		MSG* pPeekedMessage_;
	};
}
