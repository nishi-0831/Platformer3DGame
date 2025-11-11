#include "stdafx.h"
#include "ResultScene.h"
#include "../Source/ScoreViewer.h"
#include "../Source/Camera.h"

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
	Game::System<ImGuiEditorCamera>().CreateCamera();
	Camera* pCamera{ Instantiate<Camera>() };

	RectF rect{ 0,0,800,600 };
	int fontSize{ 36 };
	Instantiate<ScoreViewer>(rect, fontSize,TextAlignment::center);

	hImage = Image::Load("Image/ClearImage.png");

	CameraHandleInScene hCamera = RegisterCameraGameObject(pCamera);
	WinCtxRes::Get<CameraResource>(WindowContext::First).SetHCamera(hCamera);
}

void ResultScene::Update()
{

}

void ResultScene::Draw() const
{
	Draw::Image(hImage, draw);
}

void ResultScene::End()
{
}
