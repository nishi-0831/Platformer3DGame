#include <mtgb.h>
#include "SampleScene.h"
#include "../Source/StageManager.h"
#include "../Source/ResultScene.h"
#include "../Source/SkySphere.h"

#include "../Source/GameOverManager.h"
#include "../Source/RespawnManager.h"

namespace
{
	ImageHandle hLeftStickImg;
	ImageHandle hRightStickImg;
	ImageHandle hEastButtonPushImg;
	ImageHandle hSouthButtonPushImg;
	ImageHandle hBackgroundImg;
}

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

	hBackgroundImg = mtgb::Image::Load("Image/Black.png");
	hLeftStickImg  = mtgb::Image::Load("Image/LeftStick.png");
	hSouthButtonPushImg = mtgb::Image::Load("Image/SouthButtonPush.png");
	hRightStickImg = mtgb::Image::Load("Image/RightStick.png");
}

void SampleScene::Update()
{
	if (mtgb::InputUtil::GetKeyDown(KeyCode::ESCAPE))
	{
		mtgb::Game::System<mtgb::SceneSystem>().Move<ResultScene>();
	}
}

void SampleScene::Draw() const 
{
	RectF moveInstructionRect { 50, 550, 100, 50 };
	
	mtgb::Draw::Image(hBackgroundImg, { 50, 550, 150, 50 });
	mtgb::Draw::Image(hLeftStickImg, { 50, 550, 50, 50 });
	mtgb::Draw::ImmediateTextW(L"移動", { 100, 550, 100, 50 });

	mtgb::Draw::Image(hBackgroundImg, { 250, 550, 200, 50 });
	mtgb::Draw::Image(hSouthButtonPushImg, { 250, 550, 50, 50 });
	mtgb::Draw::ImmediateTextW(L"ジャンプ", { 300, 550, 150, 50 });

	mtgb::Draw::Image(hBackgroundImg, { 500, 550, 200, 50 });
	mtgb::Draw::Image(hRightStickImg, { 500, 550, 50, 50 });
	mtgb::Draw::ImmediateTextW(L"視点移動", { 550, 550, 150, 50 });
}

void SampleScene::End() {}
