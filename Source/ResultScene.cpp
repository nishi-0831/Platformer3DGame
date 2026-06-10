#include "stdafx.h"
#include "ResultScene.h"
#include "TitleScene.h"
#include "SkySphere.h"
#include "StageManager.h"
#include "Button.h"
#include <SerializableGameObject.h>
namespace
{
	// 118,90 , 565,100
	ImageHandle hTitleImage;
	ImageHandle hBackgroundImage;
	ImageHandle hEastButtonImg;
	RectF draw { 118, 90, 565, 100 };
	RectF textDrawRect { 118, 450, 565, 80 };
	UIParams params { .depth = 0, .layerFlag = AllLayer() };
} // namespace
ResultScene::ResultScene() {}

ResultScene::~ResultScene() {}

void ResultScene::Initialize()
{
	// エディターのカメラを作成
	Game::System<ImGuiEditorCamera>().CreateCamera();

	// シーンのカメラを生成
	GameObject* pCamera = new GameObject(GameObjectBuilder().SetPosition({ 0, 0, 0 }).SetName("SceneCamera").Build());

	// ゲームオブジェクトを管理クラスに登録
	Game::System<SceneSystem>().GetActiveScene()->RegisterGameObject(pCamera);

	// スコアを表示
	RectF rect { 0, 0, 800, 600 };

	// カメラを管理クラスに登録
	CameraHandleInScene hCamera = RegisterCameraGameObject(pCamera);
	WinCtxRes::Get<CameraResource>(WindowContext::FIRST).SetHCamera(hCamera);

	// ステージをクリアしているか判定
	bool clearedStage = Game::System<StageManager>().IsClearedCurrentStage();

	std::optional<nlohmann::json> json { std::nullopt };

	
	// クリアしているかによって表示する画像を変える
	if (clearedStage)
	{
		Game::System<Audio>().Play("GameClear");
		json		= mtgb::Game::System<StageManager>().GetStageJson(StageID::STAGE_CLEAR_SCENE);
	}
	else
	{
		Game::System<Audio>().Play("GameOver");
		json		= mtgb::Game::System<StageManager>().GetStageJson(StageID::STAGE_GAME_OVER_SCENE);
	}
	hBackgroundImage = Image::Load("Image/Black.png");
	hEastButtonImg	 = Image::Load("Image/EastButtonPush.png");

	if (json.has_value())
	{
		mtgb::GameObjectGenerator::GenerateFromJson(json);
		mtgb::Time::StabilizeDeltaTime();
	}

	Instantiate<mtgb::SkySphere>();

	CreatePanel();
}

void ResultScene::Update()
{
	panelManager_.UpdatePanel();
}

void ResultScene::Draw() const
{
	int32_t itemCount = Game::System<ScoreManager>().GetScore();
	std::string scoreText(std::to_string(itemCount));
	Draw::ImmediateText(
		scoreText,
		mtgb::RectF { 400, 320, 80, 30 },
		36,
		mtgb::TextAlignment::MIDDLE_LEFT,
		UIParams { 1, AllLayer() }
	);
}

void ResultScene::End() {}

void ResultScene::CreatePanel() 
{
	GameScene* pCurrScene = Game::System<SceneSystem>().GetActiveScene();

	Button* pItem = pCurrScene->Instantiate<Button>();
	pItem->SetOnPressed(
		[]()
		{
			Game::System<SceneSystem>().Move<TitleScene>();
		}
	);
	pItem->SetText("Return To Title");
	pItem->SetRect({ 290, 490, 220, 60 });
	Panel* pPanel = pCurrScene->Instantiate<Panel>();
	pPanel->AddUIComponent(pItem);

	panelManager_.AddPanel("ResultMenu", pPanel);
	panelManager_.EnablePanel("ResultMenu");
}
