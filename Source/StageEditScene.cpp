#include "stdafx.h"
#include <mtgb.h>
#include "StageEditScene.h"
#include "BuddiesSkyCombatStageGenerator.h"
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
	if (InputUtil::GetKeyDown(KeyCode::Space))
	{
		BuddiesSkyCombatStageGenerate();
	}
}

void StageEditScene::Draw() const
{
}

void StageEditScene::End()
{
}
