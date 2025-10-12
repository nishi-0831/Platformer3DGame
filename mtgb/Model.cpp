#include "Model.h"
#include "Fbx.h"
#include "PathUtility.h"
#include "MTAssert.h"
#include "mtgb.h"


mtgb::Model::Model()
{
}

mtgb::Model::~Model()
{
}

void mtgb::Model::Initialize()
{
}

void mtgb::Model::Update()
{
}

mtgb::ModelHandle mtgb::Model::Load(const std::string_view& _fileName)
{
	Model& instance{ Game::System<Model>() };

	// �t�@�C���̊g���q
	std::string_view fileExt{ FilePath::GetExtension(_fileName) };

	/*
	Sprite* pSprite{ new Sprite{} };
	pSprite->Initialize();
	pSprite->Load(ToWString(_fileName));
	ImageHandle handle{ ++instance.handleCounter_ };
	instance.sprites_.insert({ handle , pSprite });

	return handle;
	*/

	ModelData* pModelData{ new ModelData{} };

	if (fileExt == "fbx")
	{
		instance.modelData_.insert({  } );
	}
	else if (fileExt == "obj")
	{
		massert(false && "obj�t�@�C���͖������ł��B");
	}
	else
	{
		// fbx, obj �ł��Ȃ��Ȃ�
		massert(false && "��Ή��̃��f���t�@�C���ł��B");
	}
	return INVALID_HANDLE;
}
