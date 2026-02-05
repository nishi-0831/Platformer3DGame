#include "Fbx.h"
#include "ReleaseUtility.h"
#include "FbxParts.h"
#include "DirectX11Draw.h"
#include "MTAssert.h"

mtgb::Fbx::Fbx()
	: pFbxManager_{nullptr}
	, handleCounter_{0}
{
}

mtgb::Fbx::~Fbx()
{
	Release();
}

void mtgb::Fbx::Initialize()
{
	pFbxManager_ = FbxManager::Create();
}

void mtgb::Fbx::Update()
{
}

mtgb::FBXModelHandle mtgb::Fbx::Load(const std::string& _fileName)
{
	Fbx& instance{Game::System<Fbx>()};

	for (auto&& pFbxModel : instance.pFbxModels_)
	{
		if (pFbxModel.second->GetFileName() == _fileName)
		{
			// すでに読み込まれているならそのハンドルを返す
			return pFbxModel.first;
		}
	}

	FbxModel* pFbxModel{new FbxModel{}};

	pFbxModel->Load(_fileName);
	FBXModelHandle handle{++instance.handleCounter_};
	instance.pFbxModels_.insert({handle, pFbxModel});

	return handle;
}

void mtgb::Fbx::Draw(const FBXModelHandle _hModel, const Transform& _transfrom, int _frame)
{
	DirectX11Draw::SetBlendMode(BlendMode::Default);

	massert((0 < _hModel) && (_hModel <= handleCounter_) && "無効なハンドラ @Fbx::Draw");

	massert(pFbxModels_.count(_hModel) != 0 && "モデルが存在しない @Fbx::Draw");

	pFbxModels_[_hModel]->Draw(_transfrom, _frame);
}

void mtgb::Fbx::Release()
{
	for (auto& pFbxModel : pFbxModels_)
	{
		pFbxModel.second->Release();
	}
	pFbxModels_.clear();
	SAFE_DESTROY(pFbxManager_);
}

std::optional<FbxAnimationController> mtgb::Fbx::GetAnimationController(FBXModelHandle _hModel)
{
	Fbx& instance{Game::System<Fbx>()};

	massert((0 < _hModel) && (_hModel <= instance.handleCounter_) && "無効なハンドラ @Fbx::Draw");

	massert(instance.pFbxModels_.count(_hModel) != 0 && "モデルが存在しない @Fbx::Draw");
	return instance.pFbxModels_[_hModel]->GetAnimationController();
}

Vector3 mtgb::Fbx::GetBonePosition(FBXModelHandle _hModel, const std::string& _boneName)
{
	Fbx& instance{Game::System<Fbx>()};

	massert((0 < _hModel) && (_hModel <= instance.handleCounter_) && "無効なハンドラ @Fbx::Draw");

	massert(instance.pFbxModels_.count(_hModel) != 0 && "モデルが存在しない @Fbx::Draw");

	return instance.pFbxModels_[_hModel]->GetBonePosition(_boneName);
}

Vector3 mtgb::Fbx::GetAnimBonePosition(FBXModelHandle _hModel, const std::string& _boneName)
{
	Fbx& instance{Game::System<Fbx>()};

	massert((0 < _hModel) && (_hModel <= instance.handleCounter_) && "無効なハンドラ @Fbx::Draw");

	massert(instance.pFbxModels_.count(_hModel) != 0 && "モデルが存在しない @Fbx::Draw");

	return instance.pFbxModels_[_hModel]->GetAnimBonePosition(_boneName);
}
