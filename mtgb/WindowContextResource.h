#pragma once
#include <tuple>
#include <map>
#include <typeindex>
#include <d3d11.h>
#include "WindowContext.h"

struct IDXGISwapChain1;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID2D1RenderTarget;

namespace mtgb
{
	/// <summary>
	/// �E�B���h�E���ƂɌŗL�̃��\�[�X�̃C���^�[�t�F�[�X
	/// </summary>
	class WindowContextResource
	{
	public:
		virtual ~WindowContextResource() {};
		virtual void Initialize(WindowContext _windowContext) = 0;
		/// <summary>
		/// �E�B���h�E��ύX����ۂɌŗL�̃��\�[�X���Z�b�g����
		/// </summary>
		virtual void SetResource() = 0;

		/// <summary>
		/// �E�B���h�E�̃T�C�Y���ύX�����O�̉������
		/// </summary>
		virtual void Reset() {};
		/// <summary>
		/// �E�B���h�E�̃T�C�Y���ύX���ꂽ��̏���
		/// </summary>
		/// <param name="_width">��</param>
		/// <param name="_height">����</param>
		virtual void OnResize(WindowContext _windowContext,UINT _width, UINT _height){};
		virtual void Release() = 0;

		/// <summary>
		/// �X�V����
		/// </summary>
		virtual void Update() {};
		/// <summary>
		/// ���\�[�X�̃R�s�[���쐬�A�Ԃ�
		/// </summary>
		/// <returns>���������I�u�W�F�N�g</returns>
		virtual WindowContextResource* Clone() const = 0;
	};
}