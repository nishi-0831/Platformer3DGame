#include <mtgb.h>
#include "SampleScene.h"
#include "../mtgb/Box3D.h"
#include "../Source/StageManager.h"
#include "../Source/Player.h"
#include "../Source/ScoreViewer.h"
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
	mtgb::Game::System<ImGuiEditorCamera>().CreateCamera();

	PropertyDisplayRegistry::Instance();
	PropertyDisplayRegistry::Instance().Initialize();
	mtgb::MTImGui::Initialize();

	Instantiate<GameOverManager>();
	Instantiate<mtgb::SkySphere>();
	Instantiate<RespawnManager>();
	std::optional<nlohmann::json> json = mtgb::Game::System<StageManger>().GetStageJson(stageID_);
	if (json.has_value())
	{
		mtgb::GameObjectGenerator::GenerateFromJson(json);
		// 読み込み時間で値が大きくなったデルタタイムを安定させるために2フレーム待機させる
		// TODO: マジックナンバーを修正
		mtgb::Time::WaitFrame(2);
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
		mtgb::Game::System<SceneSystem>().Move<ResultScene>();
	}
}

void SampleScene::Draw() const {}

void SampleScene::End() {}
