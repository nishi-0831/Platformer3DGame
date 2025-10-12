#pragma once
#include <vector>
#include <map>
#include <string>
#include "ISystem.h"
#include "WindowContext.h"
#include "Vector2Int.h"
typedef struct HWND__* HWND;
typedef struct tagMSG MSG;

namespace mtgb
{
	class WindowResource;
	struct WindowConfig
	{
		std::string title;
		std::string className;
		int width;
		int height;
		int x;
		int y;
	};

	class WindowManager : public ISystem
	{
	public:
		WindowManager();
		~WindowManager();

		HWND CreateWindowContext(WindowContext context);
		Vector2Int GetWindowSize(WindowContext context);
		

		void Initialize() override;
		void Update() override;
		void Release();
		
		/// <summary>
		/// WindowContext�ɑΉ�����WindowConfig���Z�b�g
		/// </summary>
		/// <param name="windowContext">�E�B���h�E�̎��ʎq</param>
		/// <param name="config">�E�B���h�E�ݒ�</param>
		void SetWindowConfig(WindowContext windowContext, const WindowConfig& config);
		
		
		/// <summary>
		/// WindowContext����WindowConfig���擾
		/// </summary>
		/// <param name="windowContext">�E�B���h�E�̎��ʎq</param>
		/// <returns>�E�B���h�E�ݒ�</returns>
		static WindowConfig GetWindowConfig(WindowContext windowContext);
		
		
		//void SwapWindowPos(WindowContext _ctx1 = WindowContext::First, WindowContext _ctx2 = WindowContext::Second);

		static WindowResource& GetWindowResource(WindowContext windowContext);

		/// <summary>
		/// <para> �t���X�N���[����Ԃ�؂�ւ��� </para>
		/// <para> �w�肵��WindowContext�Ɋ��蓖�Ă��Ă��郂�j�^�[�̃T�C�Y�ɍ��킹�� </para>
		/// </summary>
		/// <param name="_ctx">�؂�ւ���E�B���h�E</param>
		static void ChangeFullScreenState(WindowContext _ctx);

		/// <summary>
		/// �E�B���h�E����ԋ߂��ꏊ�ɂ��郂�j�^�[�Ńt���X�N���[����Ԃ�؂�ւ���
		/// </summary>
		static void ChangeFullScreenStateNearestMonitor(WindowContext _ctx);

		/// <summary>
		/// <para> �t���X�N���[����Ԃ�؂�ւ��� </para>
		/// </summary>
		/// <param name="_ctx">�؂�ւ���E�B���h�E</param>
		/// <param name="_rect"></param>
		static void ChangeFullScreenState(WindowContext _ctx, const RECT& _rect);

		static void ResizeWindow(WindowContext _windowContext, UINT _width, UINT _height);
	private:
		static MSG* pPeekedMessage_;
		static std::map<WindowContext, WindowConfig> windowConfigMap_;
	};
}