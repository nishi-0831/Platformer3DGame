#include "stdafx.h"
#include "ResultScene.h"
#include "../Source/ScoreViewer.h"
#include "../Source/Camera.h"
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
	CameraHandleInScene hCamera = RegisterCameraGameObject(pCamera);
	WinCtxRes::Get<CameraResource>(WindowContext::First).SetHCamera(hCamera);
}

void ResultScene::Update()
{

}

void ResultScene::Draw() const
{
}

void ResultScene::End()
{
}
