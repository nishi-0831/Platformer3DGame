#include "stdafx.h"
#include <mtgb.h>
#include "TitleScene.h"
#include "Scenes/SampleScene.h"
#include "../Source/Camera.h"

namespace
{
	// 118,90 , 565,100
	ImageHandle hImage;
	RectF draw{ 118,90,565,100 };
	UIParams params{ .depth = 0,.layerFlag = AllLayer() };
}
TitleScene::TitleScene()
{
}

TitleScene::~TitleScene()
{
}

void TitleScene::Initialize()
{
	Game::System<ImGuiEditorCamera>().CreateCamera();
	GameObject* pCamera = new GameObject(
		GameObjectBuilder()
		.SetPosition({ 0,0,0 })
		.SetName("SceneCamera")
		.Build());
	Game::System<SceneSystem>().GetActiveScene()->RegisterGameObject(pCamera);

	hImage = Image::Load("Image/TitleImage.png");

	CameraHandleInScene hCamera = RegisterCameraGameObject(pCamera);
	WinCtxRes::Get<CameraResource>(WindowContext::First).SetHCamera(hCamera);
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
	Draw::Image(hImage, draw);

}

void TitleScene::End()
{
}
