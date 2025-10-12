#include "FbxModel.h"
#include "FbxParts.h"
#include "ReleaseUtility.h"
#include "Fbx.h"
#include "MTAssert.h"


mtgb::FbxModel::FbxModel() :
	animationSpeed_{ 1.0f },
	startFrame_{ 0 },
	endFrame_{ 0 },
	frameRate_{ FbxTime::EMode::eFrames60 },
	pFbxScene_{ nullptr }
{
}

mtgb::FbxModel::~FbxModel()
{
	
}

void mtgb::FbxModel::Load(const std::string& _fileName)
{
	massert(pFbxScene_ == nullptr  // �܂��ǂݍ��܂�Ă��Ȃ�
		&& "����FbxModel�͓ǂݍ��܂�Ă����I @FbxModel::Load");

	FbxManager* pFbxManager{ Game::System<Fbx>().GetFbxManager() };

	pFbxScene_ = FbxScene::Create(pFbxManager, "fbxscene");
	FbxString fileName{ _fileName.c_str() };
	FbxImporter* fbxImporter{ FbxImporter::Create(pFbxManager, "imp") };

	FbxIOSettings* ios = FbxIOSettings::Create(pFbxManager, IOSROOT);

	pFbxManager->SetIOSettings(ios);
	if (!fbxImporter->Initialize(fileName.Buffer(), -1, ios)) {
		MessageBoxA(NULL, fbxImporter->GetStatus().GetErrorString(), "FBX Import Error", MB_OK);
		// �������̓��O�o��
	}
	//none of the registered readers can process the file

	fileName_ = _fileName;


	char str[MAX_PATH]{};
	GetCurrentDirectory(MAX_PATH, str);

	bool succeed{ false };
	//succeed = fbxImporter->Initialize(fileName.Buffer(), -1);
	//massert(succeed	&& "fbxImporter�̏������Ɏ��s���� @Fbx::Load");
	succeed = fbxImporter->Initialize(fileName.Buffer(), -1, pFbxManager->GetIOSettings());
	massert(succeed && "fbxImporter�̏������Ɏ��s���� @Fbx::Load");

	succeed = fbxImporter->Import(pFbxScene_);
	massert(succeed && "�ǂݍ��݂Ɏ��s���� @Fbx::Load");

	SAFE_DESTROY(fbxImporter);  // �C���|�[�^�͉��

	// 3�p�|���S��
	FbxGeometryConverter geometryConverter{ pFbxManager };

	// �A�j���[�V�����^�C�����[�h�̎擾
	frameRate_ = pFbxScene_->GetGlobalSettings().GetTimeMode();

	// ���݂̃J�����g�f�B���N�g�����擾
	char defaultCurrentDirectory[MAX_PATH]{};
	GetCurrentDirectory(MAX_PATH, defaultCurrentDirectory);

	// �J�����g�f�B���N�g�����ړ�
	char directory[MAX_PATH]{};
	_splitpath_s(_fileName.c_str(), nullptr, 0, directory, MAX_PATH, nullptr, 0, nullptr, 0);
	SetCurrentDirectory(directory);

	int meshCount{ pFbxScene_->GetSrcObjectCount<FbxMesh>() };
	for (int i = 0; i < meshCount; i++)
	{
		FbxMesh* pMesh = pFbxScene_->GetSrcObject<FbxMesh>(i);
		if (pMesh == nullptr) continue;

		FbxNode* pNode = pMesh->GetNode();
		if (pNode == nullptr) continue;

		// �쐬�O�Ƀm�[�h�̃��b�V���L���m�F
		if (pNode->GetMesh() == nullptr) continue;

		FbxParts* pParts = new FbxParts(pNode);
		pParts->Initialize();
		pParts_.push_back(pParts);
	}

	SetCurrentDirectory(defaultCurrentDirectory);

}

void mtgb::FbxModel::Draw(const Transform& _transform, int _frame)
{
	for (int i = 0; i < pParts_.size(); i++)
	{
		// �A�j���[�V�����^�C���̎p���s����擾����
		FbxTime time;
		time.SetTime(0, 0, 0, _frame, 0, 0, frameRate_);

		// �{�[��������X�L���A�j���[�V����
		if (pParts_[i]->GetSkin() != nullptr)
		{
			pParts_[i]->DrawSkinAnimation(_transform, time);
		}
		else  // ���b�V���A�j���[�V����
		{
			pParts_[i]->DrawMeshAnimation(_transform, time);
		}
	}
}

void mtgb::FbxModel::Release()
{
	for (auto fbxParts : pParts_)
	{
		fbxParts->Release();
		delete fbxParts;
	}
}

mtgb::Vector3 mtgb::FbxModel::GetBonePosition(std::string _boneName)
{
	Vector3 position_ = Vector3(0, 0, 0);
	for (int i = 0; i < pParts_.size(); i++)
	{
		if (pParts_[i]->TryGetBonePosition(_boneName, &position_))
			break;
	}
	return position_;
}

mtgb::Vector3 mtgb::FbxModel::GetAnimBonePosition(std::string _boneName)
{
	Vector3 position_ = Vector3(0, 0, 0);
	for (int i = 0; i < pParts_.size(); i++)
	{
		if (pParts_[i]->TryGetBonePositionAtNow(_boneName, &position_))
			break;
	}
	return position_;
}

void mtgb::FbxModel::CheckNode(FbxNode* _pNode, std::vector<FbxParts*>& _pPartsList)
{
	// �m�[�h�̑������
	FbxNodeAttribute* pNodeAttribute{ _pNode->GetNodeAttribute() };

	if (pNodeAttribute == nullptr)
	{
		return;  // �m�[�h�� nullptr �Ȃ��A
	}

	// ���b�V���̏�񂪓����Ă���Ȃ�
	if (pNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
	{
		FbxParts* pParts = new FbxParts(_pNode);
		pParts->Initialize();
		_pPartsList.push_back(pParts);
	}

	// �q�̐�
	int childCount{ _pNode->GetChildCount() };

	// 1���`�F�b�N
	for (int i = 0; i < childCount; i++)
	{
		CheckNode(_pNode->GetChild(i), _pPartsList);
	}
}

