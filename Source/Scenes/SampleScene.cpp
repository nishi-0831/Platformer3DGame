#include <mtgb.h>
#include "SampleScene.h"
#include "../Source/StageManager.h"
#include "../Source/ResultScene.h"
#include "../Source/SkySphere.h"

#include "../Source/GameOverManager.h"
#include "../Source/RespawnManager.h"
#include <SerializableGameObject.h>
namespace
{
}

SampleScene::SampleScene()
	: stageID_ { StageID::STAGE_ONE }
{
}

SampleScene::~SampleScene() {}

void SampleScene::Initialize()
{
	using namespace mtgb;

	mtgb::Game::System<mtgb::ImGuiEditorCamera>().CreateCamera();
	mtgb::Game::System<mtgb::Audio>().Play("PlayScene", true);
	mtgb::Game::System<mtgb::SceneSystem>().OnMove(
		[]()
		{
			mtgb::Game::System<mtgb::Audio>().Stop("PlayScene");
		}
	);

	Instantiate<GameOverManager>();
	Instantiate<mtgb::SkySphere>();
	Instantiate<RespawnManager>();
	mtgb::Game::System<ScoreManager>().ResetScore();
	mtgb::Game::System<StageManager>().StartStage(stageID_);
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
