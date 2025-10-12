#pragma once
#include <vector>
#include "Vector3.h"
#include <fbxsdk.h>
#include <string>

#include "IModelPack.h"


namespace mtgb
{
	class FbxParts;

	/// <summary>
	/// 1��Fbx�t�@�C���������N���X
	/// </summary>
	class FbxModel : public IModelPack
	{
		friend class FbxParts;

	public:
		FbxModel();
		~FbxModel();

		void Load(const std::string& _fileName) override;

		void Draw(const Transform& _transfrom, int _frame) override;

		void Release() override;

		/// <summary>
		/// FbxScene���擾����
		/// </summary>
		/// <returns>FbxScene�̃|�C���^</returns>
		inline FbxScene* GetFbxScene() { return pFbxScene_; }

		/// <summary>
		/// Fbx�p�[�c���擾����
		/// </summary>
		/// <param name="_index">�C���f�b�N�X</param>
		/// <returns>Fbx�p�[�c�̃|�C���^</returns>
		inline FbxParts* GetFbxParts(const int _index) { return pParts_[_index]; }

		/// <summary>
		/// �C�ӂ̃{�[���̈ʒu���擾
		/// </summary>
		/// <param name="_boneName">�{�[���̖��O</param>
		Vector3 GetBonePosition(std::string _boneName);
		/// <summary>
		/// �X�L�����b�V���A�j�����̌��݂̔C�ӂ̃{�[���̈ʒu���擾
		/// </summary>
		/// <param name="_boneName">�{�[���̖��O</param>
		Vector3 GetAnimBonePosition(std::string _boneName);
		
		/// <summary>
		/// ���[�h�����Ƃ��̃��f���t�@�C���̖��O���擾
		/// </summary>
		/// <returns>�t�@�C����</returns>
		std::string GetFileName() const { return fileName_; }
	private:

		/// <summary>
		/// �m�[�h�̒��g�𒲂ׂ�
		/// </summary>
		/// <param name="_pNode">���ׂ����m�[�h</param>
		/// <param name="_parts">�p�[�c���X�g</param>
		void CheckNode(FbxNode* _pNode, std::vector<FbxParts*>& _parts);


	private:
		std::vector<FbxParts*> pParts_;  // �������邩������Ȃ��p�[�c

		FbxScene* pFbxScene_;  // Fbx�t�@�C�����̃V�[��

		FbxTime::EMode frameRate_;  // �A�j���[�V�����t���[�����[�g
		float animationSpeed_;  // �A�j���[�V�����̍Đ����x
		int startFrame_;  // �A�j���[�V�����ŏ��̃t���[��
		int endFrame_;  // �A�j���[�V�����Ō�̃t���[��

		std::string fileName_;  // ���f���t�@�C���̃p�X
	};
}
