#include "stdafx.h"
#include <mtgb.h>
#include "StageEditScene.h"

StageEditScene::StageEditScene()
{
}

StageEditScene::~StageEditScene()
{
}

void StageEditScene::Initialize()
{
	Game::System<ImGuiEditorCamera>().CreateCamera();

	TypeRegistry::Instance();
	TypeRegistry::Instance().Initialize();
	MTImGui::Instance().Initialize();
}

void StageEditScene::Update()
{
}

void StageEditScene::Draw() const
{
}

void StageEditScene::End()
{
}
