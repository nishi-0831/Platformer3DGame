#include "stdafx.h"
#include <mtgb.h>
#include <fstream>
#include "StageEditScene.h"
#include "Scenes/SampleScene.h"
StageEditScene::StageEditScene()
	: stageData_ {}
{
}

StageEditScene::StageEditScene(const nlohmann::json& _stageData)
{
	stageData_ = _stageData;
}

StageEditScene::~StageEditScene() {}

void StageEditScene::Initialize()
{
	Game::SetEditMode(true);
	Game::System<ImGuiEditorCamera>().CreateCamera();
	mtgb::GameObjectGenerator::Initialize();
	PropertyDisplayRegistry::Instance();
	PropertyDisplayRegistry::Instance().Initialize();
	MTImGui::Initialize();
	if (stageData_.empty() == false)
	{
		mtgb::GameObjectGenerator::GenerateFromJson(stageData_);
		mtgb::Time::StabilizeDeltaTime();
	}
}

void StageEditScene::Update() {}

void StageEditScene::Draw() const {}

void StageEditScene::End() {}
