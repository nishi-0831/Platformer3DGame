#include <mtgb.h>
#include "SampleScene.h"
#include "../mtgb/Box3D.h"
#include "../Source/StageManager.h"
#include "../Source/Camera.h"
#include "../Source/Player.h"
#include "../Source/ScoreViewer.h"
#include "../Source/ResultScene.h"
#include "../Source/SkySphere.h"
#include "../Source/MovingFloor.h"
#include "../Source/GameOverManager.h"

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

	PropertyDisplayRegistry::Instance();
	PropertyDisplayRegistry::Instance().Initialize();
	MTImGui::Instance().Initialize();

	Instantiate<GameOverManager>();
	Instantiate<SkySphere>();
	std::optional<nlohmann::json> json = Game::System<StageManger>().GetStageJson(stageID_);
	if (json.has_value())
	{
		GameObjectGenerator::GenerateFromJson(json);
		// 読み込み時間で値が大きくなったデルタタイムを安定させるために2フレーム待機させる
		// TODO: マジックナンバーを修正
		Time::WaitFrame(2);
	}
	else
	{
		assert(false && "JSONファイルが見つかりません");
	}
}

void SampleScene::Update()
{
	if (InputUtil::GetKeyDown(KeyCode::ESCAPE))
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
