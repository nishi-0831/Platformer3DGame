#include "FbxModel.h"
#include "Utility/ReleaseUtility.h"
#include "Model/Fbx.h"
#include "Utility/MTAssert.h"
#include "ShaderManager.h"
mtgb::FbxModel::FbxModel()
	: pFbxScene_ { nullptr }
	, frameRate_ { FbxTime::EMode::eFrames60 }
	, animationSpeed_ { 1.0f }
	, startFrame_ { 0 }
	, endFrame_ { 0 }
	, unitScaleFactor_ { 1.0 }
{
}

mtgb::FbxModel::~FbxModel() {}

void mtgb::FbxModel::Load(const std::string& _fileName)
{
	massert(
		pFbxScene_ == nullptr // まだ読み込まれていない
		&& "既にFbxModelは読み込まれているよ！ @FbxModel::Load"
	);

	FbxManager* pFbxManager { Game::System<Fbx>().GetFbxManager() };

	pFbxScene_ = FbxScene::Create(pFbxManager, "fbxscene");

	FbxString fileName { _fileName.c_str() };
	FbxImporter* fbxImporter { FbxImporter::Create(pFbxManager, "imp") };

	FbxIOSettings* ios = FbxIOSettings::Create(pFbxManager, IOSROOT);

	pFbxManager->SetIOSettings(ios);
	if (!fbxImporter->Initialize(fileName.Buffer(), -1, ios))
	{
		MessageBoxA(NULL, fbxImporter->GetStatus().GetErrorString(), "FBX Import Error", MB_OK);
		// もしくはログ出力
	}

	fileName_ = _fileName;

	char str[MAX_PATH] {};
	GetCurrentDirectory(MAX_PATH, str);

	bool succeed { false };
	succeed = fbxImporter->Initialize(fileName.Buffer(), -1, pFbxManager->GetIOSettings());
	massert(succeed && "fbxImporterの初期化に失敗した @Fbx::Load");

	succeed = fbxImporter->Import(pFbxScene_);
	massert(succeed && "読み込みに失敗した @Fbx::Load");

	SAFE_DESTROY(fbxImporter); // インポータは解放

	// DirectXの座標系に変換
	FbxAxisSystem sceneAxisSystem = pFbxScene_->GetGlobalSettings().GetAxisSystem();
	FbxAxisSystem ourAxisSystem(FbxAxisSystem::DirectX); // DirectX = Y-Up, Left-Handed
	if (sceneAxisSystem != ourAxisSystem)
	{
		// MEMO: DeepConvertにすることでシーン全体を変換できる。Convertだとルートのノードだけ
		ourAxisSystem.DeepConvertScene(pFbxScene_);
	}

	// 3角ポリゴン
	FbxGeometryConverter geometryConverter { pFbxManager };

	// アニメーションタイムモードの取得
	frameRate_ = pFbxScene_->GetGlobalSettings().GetTimeMode();

	// スケール単位を取得
	unitScaleFactor_ = pFbxScene_->GetGlobalSettings().GetSystemUnit().GetScaleFactor();
	// 現在のカレントディレクトリを取得
	char defaultCurrentDirectory[MAX_PATH] {};
	GetCurrentDirectory(MAX_PATH, defaultCurrentDirectory);

	// カレントディレクトリを移動
	char directory[MAX_PATH] {};
	_splitpath_s(_fileName.c_str(), nullptr, 0, directory, MAX_PATH, nullptr, 0, nullptr, 0);
	SetCurrentDirectory(directory);

	int meshCount { pFbxScene_->GetSrcObjectCount<FbxMesh>() };
	for (int i = 0; i < meshCount; i++)
	{
		FbxMesh* pMesh = pFbxScene_->GetSrcObject<FbxMesh>(i);
		if (pMesh == nullptr)
			continue;

		FbxNode* pNode = pMesh->GetNode();
		if (pNode == nullptr)
			continue;

		// 作成前にノードのメッシュ有無確認
		if (pNode->GetMesh() == nullptr)
			continue;

		MeshAsset* pMeshAsset = MeshAsset::LoadFromFbx(pNode, unitScaleFactor_);
		pMeshAsset->CreateGpuResources(DirectX11Draw::pDevice_.Get());
		pMeshAssets_.push_back(pMeshAsset);
	}

	SetCurrentDirectory(defaultCurrentDirectory);
}

void mtgb::FbxModel::Draw(const Transform& _transform, int _frame, ShaderType _shader)
{
	IShader& shader = Game::System<ShaderManager>().GetShader(_shader);

	for (int i = 0; i < pMeshAssets_.size(); i++)
	{
		// アニメーションタイムの姿勢行列を取得する
		FbxTime time;
		time.SetTime(0, 0, 0, _frame, 0, 0, frameRate_);
		shader.Bind(DirectX11Draw::pContext_.Get());
		shader.Draw(DirectX11Draw::pContext_.Get(), _transform, pMeshAssets_[i], _frame);
	}
}

void mtgb::FbxModel::Release()
{
	for (auto asset : pMeshAssets_)
	{
		asset->ReleaseGpuResources();
		delete asset;
	}
}

mtgb::Vector3 mtgb::FbxModel::GetBonePosition(std::string_view _boneName)
{
	Vector3 position_ = Vector3(0, 0, 0);
	for (int i = 0; i < pMeshAssets_.size(); i++)
	{
		if (pMeshAssets_[i]->TryGetBonePosition(_boneName, &position_))
			break;
	}
	return position_;
}

mtgb::Vector3 mtgb::FbxModel::GetAnimBonePosition(std::string_view _boneName)
{
	Vector3 position_ = Vector3(0, 0, 0);
	for (int i = 0; i < pMeshAssets_.size(); i++)
	{
		if (pMeshAssets_[i]->TryGetBonePositionAtNow(_boneName, &position_))
			break;
	}
	return position_;
}

std::optional<mtgb::FbxAnimationController> mtgb::FbxModel::GetAnimationController()
{
	return FbxAnimationController(pFbxScene_, fileName_);
}
