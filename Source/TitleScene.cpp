#include "stdafx.h"
#include <mtgb.h>
#include "TitleScene.h"
#include "Scenes/SampleScene.h"
#include "../Source/SkySphere.h"
#include "../Source/StageManager.h"

namespace
{
	// 118,90 , 565,100
	ImageHandle hTitleImage;
	ImageHandle hBackgroundImage;
	FBXModelHandle hModel;
	RectF draw { 118, 90, 565, 100 };
	RectF textDrawRect { 118, 450, 565, 80 };
} // namespace
TitleScene::TitleScene() {}

TitleScene::~TitleScene() {}

void TitleScene::Initialize()
{
	Game::System<ImGuiEditorCamera>().CreateCamera();

	GameObject* pCamera = new GameObject(GameObjectBuilder().SetPosition({ 0, 0, 0 }).SetName("SceneCamera").Build());
	Game::System<SceneSystem>().GetActiveScene()->RegisterGameObject(pCamera);
	hTitleImage		 = Image::Load("Image/TitleImage.png");
	hBackgroundImage = Image::Load("Image/Black.png");

	CameraHandleInScene hCamera = RegisterCameraGameObject(pCamera);
	WinCtxRes::Get<CameraResource>(WindowContext::FIRST).SetHCamera(hCamera);

	std::optional<nlohmann::json> json = mtgb::Game::System<StageManager>().GetStageJson(StageID::STAGE_TITLE_SCENE);
	if (json.has_value())
	{
		mtgb::GameObjectGenerator::GenerateFromJson(json);
		mtgb::Time::StabilizeDeltaTime();
	}
	Instantiate<mtgb::SkySphere>();
}

void TitleScene::Update()
{
	if (InputUtil::GetKeyDown(KeyCode::P))
	{
		Game::System<SceneSystem>().Move<SampleScene>();
	}
}

void TitleScene::Draw() const
{
	Draw::Image(hTitleImage, draw);
	Draw::Image(hBackgroundImage, textDrawRect, mtgb::UIParams {}, mtgb::Color(0, 0, 0, 90));
	Draw::ImmediateText("push P Key to start game...", textDrawRect);
}

void TitleScene::End() {}
