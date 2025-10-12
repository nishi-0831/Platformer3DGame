#pragma once
#include "WindowContextResourceManager.h"
#include "Game.h"
#include "ISystem.h"
namespace mtgb
{
	using WinCtxResManager = WindowContextResourceManager;
	
	/// <summary>
	/// WindowContextResource
	/// ���b�p�[�A��ۃN���X�ˑ��֗̕��֐��Q
	/// </summary>
	namespace WinCtxRes
	{
		/// <summary>
		/// ���\�[�X���擾����
		/// </summary>
		/// <typeparam name="ResourceT">���\�[�X�̌^</typeparam>
		/// <param name="ctx">�擾����E�B���h�E�̎��ʎq</param>
		/// <returns></returns>
		template<typename ResourceT>
		ResourceT& Get(WindowContext ctx)
		{
			return Game::System<WinCtxResManager>().Get<ResourceT>(ctx);
		}

		/// <summary>
		/// ���\�[�X��؂�ւ���
		/// </summary>
		/// <param name="ctx">�؂�ւ���E�B���h�E�̎��ʎq</param>
		void ChangeResource(WindowContext ctx);

		/// <summary>
		/// ���ݗL���ȃE�B���h�E�̎��ʎq��Ԃ�
		/// </summary>
		/// <returns>���݂�WindowContext</returns>
		WindowContext CurrContext();

		/// <summary>
		/// ���̃E�B���h�E��HWND���擾����
		/// </summary>
		/// <param name="ctx">�E�B���h�E�̎��ʎq</param>
		/// <returns></returns>
		HWND GetHWND(WindowContext ctx);

		/// <summary>
		/// <para> ��̃E�B���h�E�̕`�惊�\�[�X��؂�ւ��� </para>
		/// <para> ���͂�J�����Ȃǂ͐؂芷���Ȃ� </para>
		/// </summary>
		/// <param name="_ctx1"></param>
		/// <param name="_ctx2"></param>
		void SwapWindow(WindowContext _ctx1 = WindowContext::First, WindowContext _ctx2 = WindowContext::Second);
	}
}