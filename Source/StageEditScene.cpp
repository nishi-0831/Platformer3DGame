#include "stdafx.h"
#include <mtgb.h>
#include <fstream>
#include "StageEditScene.h"
#include "Scenes/SampleScene.h"
StageEditScene::StageEditScene() {}

StageEditScene::~StageEditScene() {}

void StageEditScene::Initialize()
{
	Game::System<ImGuiEditorCamera>().CreateCamera();
	mtgb::GameObjectGenerator::Initialize();
	PropertyDisplayRegistry::Instance();
	PropertyDisplayRegistry::Instance().Initialize();
	MTImGui::Initialize();
}

void StageEditScene::Update() 
{
}

void StageEditScene::Draw() const {}

void StageEditScene::End() {}
