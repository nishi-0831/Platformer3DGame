#pragma once
#include <vector>
#include <map>
#include <fbxsdk.h>
#include "Vector3.h"
#include "IModelPack.h"
#include "ISystem.h"
#include "FbxModel.h"
#include "Handlers.h"

#pragma comment(lib, "LibFbxSDK-MT.lib")
#pragma comment(lib, "LibXml2-MT.lib")
#pragma comment(lib, "zlib-MT.lib")

namespace mtgb
{
	class FbxParts;
	/// <summary>
	/// 3D���f����Fbx��ǂݍ��݂���N���X
	/// </summary>
	class Fbx : public ISystem
	{
		friend class FbxParts;

	public:
		Fbx();
		~Fbx();

		void Initialize() override;
		void Update() override;

		/// <summary>
		/// FbxManager���擾����
		/// </summary>
		/// <returns>FbxManager�̃|�C���^</returns>
		inline FbxManager* GetFbxManager() { return pFbxManager_; }
		

		/// <summary>
		/// Fbx�t�@�C����ǂݍ��݂���
		/// </summary>
		/// <param name="_fileName">3D���f���̃t�@�C����</param>
		static FBXModelHandle Load(const std::string& _fileName);
		/// <summary>
		/// Fbx���f����`�悷��
		/// </summary>
		/// <param name="_transfrom">���W�n</param>
		/// <param name="_frame">�A�j���[�V�����t���[��</param>
		void Draw(const FBXModelHandle _hModel, const Transform& _transfrom, int _frame);
		/// <summary>
		/// �������
		/// </summary>
		void Release();

		

	private:
		FbxManager* pFbxManager_;  // Fbx�t�@�C���������{��

		FBXModelHandle handleCounter_;              // �n���h���̉��Z�p
		std::map<FBXModelHandle, FbxModel*> pFbxModels_;  // �ǂݍ��܂ꂽ�X�v���C�g
	};
}
