#include <mtgb.h>
#include "SampleScene.h"
#include "../mtgb/Box3D.h"
#include "../Source/StageManager.h"
#include "../Source/Camera.h"
#include "../Source/Player.h"
#include "../Source/ScoreViewer.h"
#include "../Source/ResultScene.h"

SampleScene::SampleScene()
	: stageID_{StageID::STAGE_ONE}
{
}

SampleScene::~SampleScene()
{
}

void SampleScene::Initialize()
{
	Game::System<ImGuiEditorCamera>().CreateCamera();

	TypeRegistry::Instance();
	TypeRegistry::Instance().Initialize();
	MTImGui::Instance().Initialize();

	std::optional<nlohmann::json> json = Game::System<StageManger>().GetStageJson(stageID_);
	if (json.has_value())
	{
		GameObjectGenerator::GenerateFromJson(json);
	}
	else
	{
		assert(false);
	}
}

void SampleScene::Update()
{
	if (InputUtil::GetKeyDown(KeyCode::Escape))
	{
		Game::Exit();
	}
}

void SampleScene::Draw() const
{
}

void SampleScene::End()
{
}
