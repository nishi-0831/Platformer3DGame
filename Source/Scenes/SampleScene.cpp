#include <mtgb.h>
#include "SampleScene.h"
#include "../mtgb/Box3D.h"
//#include "../Source/SampleGround.h"
#include "../Source/Camera.h"
SampleScene::SampleScene()
{
}

SampleScene::~SampleScene()
{
}

void SampleScene::Initialize()
{
	Game::System<ImGuiEditorCamera>().CreateCamera();

	TypeRegistry::Instance();
	TypeRegistry::Instance().Initialize();
	MTImGui::Instance().Initialize();

	Instantiate<Box3D>();

	Camera* pCamera{ Instantiate<Camera>() };
	CameraHandleInScene hCamera = RegisterCameraGameObject(pCamera);

	WinCtxRes::Get<CameraResource>(WindowContext::First).SetHCamera(hCamera);
}

void SampleScene::Update()
{
	if (InputUtil::GetKeyDown(KeyCode::Escape))
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
