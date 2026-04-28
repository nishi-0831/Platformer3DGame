#include "stdafx.h"
#include "ResultScene.h"
#include "../Source/TitleScene.h"
#include "../Source/StageManager.h"
namespace
{
	// 118,90 , 565,100
	ImageHandle hTitleImage;
	ImageHandle hBackgroundImage;
	RectF draw { 118, 90, 565, 100 };
	RectF textDrawRect { 118, 400, 565, 100 };
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
	int fontSize { 36 };

	// カメラを管理クラスに登録
	CameraHandleInScene hCamera = RegisterCameraGameObject(pCamera);
	WinCtxRes::Get<CameraResource>(WindowContext::FIRST).SetHCamera(hCamera);

	// ステージをクリアしているか判定
	bool clearedStage = Game::System<StageManger>().IsClearedCurrentStage();

	// クリアしているかによって表示する画像を変える
	if (clearedStage)
	{
		hTitleImage = Image::Load("Image/ClearImage.png");
	}
	else
	{
		hTitleImage = Image::Load("Image/GameOverImage.png");
	}
	hBackgroundImage = Image::Load("Image/Black.png");
}

void ResultScene::Update()
{
	if (InputUtil::GetKeyDown(KeyCode::P))
	{
		Game::System<SceneSystem>().Move<TitleScene>();
	}
}

void ResultScene::Draw() const
{
	Draw::Image(hTitleImage, draw);
	Draw::Image(hBackgroundImage, textDrawRect, mtgb::UIParams {}, mtgb::Color(0, 0, 0, 127));
	Draw::ImmediateText("push P Key to return title...", textDrawRect);

	Draw::Image(hBackgroundImage, mtgb::RectF { 120, 305, 310, 40 }, mtgb::UIParams {}, mtgb::Color(0, 0, 0, 127));

	int32_t itemCount = Game::System<ScoreManager>().GetScore();
	Draw::ImmediateText("Items Collected", mtgb::RectF { 120, 320, 300, 30 }, 36, mtgb::TextAlignment::MIDDLE_LEFT);
	std::string scoreText(std::to_string(itemCount));
	Draw::ImmediateText(scoreText, mtgb::RectF { 400, 320, 80, 30 }, 36, mtgb::TextAlignment::MIDDLE_LEFT);
}

void ResultScene::End() {}
