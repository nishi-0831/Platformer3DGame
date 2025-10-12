#pragma once
#include <string>
#include "Vector2Int.h"

#pragma comment(lib, "WindowsCodecs.lib")
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

struct ID3D11SamplerState;
struct ID3D11ShaderResourceView;
struct D3D11_TEXTURE2D_DESC;

namespace mtgb
{
	/// <summary>
	/// 2D�̃e�N�X�`��
	/// </summary>
	class Texture2D
	{
	public:
		Texture2D();
		~Texture2D();

		/// <summary>
		/// �摜�t�@�C������e�N�X�`�������[�h����
		/// </summary>
		/// <param name="fileName">�t�@�C���̃p�X</param>
		void Load(const std::wstring& _fileName);

		/// <summary>
		/// �T���v���[�X�e�[�g���擾
		/// </summary>
		/// <returns>�T���v���[�X�e�[�g�̃|�C���^</returns>
		inline ID3D11SamplerState* GetSamplerState() { return pSamplerLinear_.Get(); }
		/// <summary>
		/// �V�F�[�_���\�[�X�r�����擾
		/// </summary>
		/// <returns>�V�F�[�_���\�[�X�r���̃|�C���^</returns>
		inline ID3D11ShaderResourceView* GetShaderResourceView() { return pShaderResourceView_.Get(); }
	
		/// <summary>
		/// �e�N�X�`���̉摜�̃T�C�Y���擾
		/// </summary>
		/// <returns>���R���ƃ^�e���̃s�N�Z��</returns>
		inline const Vector2Int GetSize() const { return size_; }
	
	private:
		ComPtr<ID3D11SamplerState> pSamplerLinear_;  // �\���
		ComPtr<ID3D11ShaderResourceView> pShaderResourceView_;  // �V�F�[�_�ɑ��邽�߂̂���
		Vector2Int size_;  // �摜�̃T�C�Y
	};
}
