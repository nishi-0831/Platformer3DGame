#include "stdafx.h"
#include "ResultScene.h"
//#include "../Source/ScoreViewer.h"
#include "../Source/Camera.h"
#include "../Source/TitleScene.h"
#include "../Source/StageManager.h"
namespace
{
	// 118,90 , 565,100
	ImageHandle hImage;
	RectF draw{118,90,565,100};
	UIParams params{ .depth = 0,.layerFlag = AllLayer() };
}
ResultScene::ResultScene()
{
}

ResultScene::~ResultScene()
{
}

void ResultScene::Initialize()
{
	// エディターのカメラを作成
	Game::System<ImGuiEditorCamera>().CreateCamera();

	// シーンのカメラを生成
	GameObject* pCamera = new GameObject(
		GameObjectBuilder()
		.SetPosition({ 0,0,0 })
		.SetName("SceneCamera")
		.Build());

	// ゲームオブジェクトを管理クラスに登録
	Game::System<SceneSystem>().GetActiveScene()->RegisterGameObject(pCamera);

	// スコアを表示
	RectF rect{ 0,0,800,600 };
	int fontSize{ 36 };
	//Instantiate<ScoreViewer>(rect, fontSize,TextAlignment::center);

	// カメラを管理クラスに登録
	CameraHandleInScene hCamera = RegisterCameraGameObject(pCamera);
	WinCtxRes::Get<CameraResource>(WindowContext::First).SetHCamera(hCamera);

	// ステージをクリアしているか判定
	bool clearedStage = Game::System<StageManger>().IsClearedCurrentStage();
	
	// クリアしているかによって表示する画像を変える
	if (clearedStage)
	{
		hImage = Image::Load("Image/ClearImage.png");
	}
	else
	{
		hImage = Image::Load("Image/GameOverImage.png");
	}
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
	Draw::Image(hImage, draw);
}

void ResultScene::End()
{
}
