#pragma once
#include <string>


namespace mtgb
{
	class Transform;

	/// <summary>
	/// 3D���f���ǂݍ��ݗp���ۃC���^�t�F�[�X
	/// </summary>
	class IModelPack
	{
	public:
		IModelPack() {}
		virtual ~IModelPack() {}

		/// <summary>
		/// 3D���f���̓ǂݍ��݂�����
		/// </summary>
		/// <param name="_fileName">3D���f���̃t�@�C����</param>
		virtual void Load(const std::string& _fileName) = 0;
		/// <summary>
		/// 3D���f����`�悷��
		/// </summary>
		/// <param name="_transfrom">���W�n</param>
		/// <param name="_frame">�A�j���[�V�����t���[��</param>
		virtual void Draw(const Transform& _transfrom, int _frame) = 0;
		/// <summary>
		/// �������
		/// </summary>
		virtual void Release() = 0;
	};
}
