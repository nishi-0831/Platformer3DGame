#include <mtgb.h>
#include "SampleScene.h"
#include "../Source/StageManager.h"
#include "../Source/ResultScene.h"
#include "../Source/SkySphere.h"

#include "../Source/GameOverManager.h"
#include "../Source/RespawnManager.h"

SampleScene::SampleScene()
	: stageID_ { StageID::STAGE_ONE }
{
}

SampleScene::~SampleScene() {}

void SampleScene::Initialize()
{
	mtgb::Game::System<mtgb::ImGuiEditorCamera>().CreateCamera();

	Instantiate<GameOverManager>();
	Instantiate<mtgb::SkySphere>();
	Instantiate<RespawnManager>();
	std::optional<nlohmann::json> json = mtgb::Game::System<StageManager>().GetStageJson(stageID_);
	if (json.has_value())
	{
		mtgb::GameObjectGenerator::GenerateFromJson(json);
		mtgb::Time::StabilizeDeltaTime();
	}
	else
	{
		assert(false && "JSONファイルが見つかりません");
	}
}

void SampleScene::Update()
{
	if (mtgb::InputUtil::GetKeyDown(KeyCode::ESCAPE))
	{
		mtgb::Game::System<mtgb::SceneSystem>().Move<ResultScene>();
	}
}

void SampleScene::Draw() const {}

void SampleScene::End() {}
