#pragma once
#include <d3dcompiler.h>

namespace mtgb
{
	/// <summary>
	/// �V�F�[�_�w�b�_�ǂݍ��ݗp - �p�����Ă���
	/// </summary>
	struct HLSLInclude : public ID3DInclude
	{
		/// <summary>
		/// �J���Ƃ��̏���
		/// </summary>
		/// <param name="_includeType">�C���N���[�h�̃^�C�v</param>
		/// <param name="_pFileNamm">�ǂݍ��ރt�@�C���p�X</param>
		/// <param name="_pParentData">�e�f�[�^�̃|�C���^�n��</param>
		/// <param name="_ppData">�f�[�^�̃|�C���^�̃|�C���^�n��</param>
		/// <param name="_pBytes">�o�C�g���̃|�C���^�n��</param>
		/// <returns>���ʃn���h��</returns>
		HRESULT Open(
			D3D_INCLUDE_TYPE _includeType,
			LPCSTR _pFileName,
			LPCVOID _pParentData,
			LPCVOID* _ppData,
			UINT* _pBytes) override;

		/// <summary>
		/// ����Ƃ��̏���
		/// </summary>
		/// <param name="_pData">�f�[�^�̃|�C���^�n��</param>
		/// <returns>���ʃn���h��</returns>
		HRESULT Close(LPCVOID _pData) override;
	};
}
