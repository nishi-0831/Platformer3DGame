#include <mtgb.h>
#include "SampleScene.h"
#include "../mtgb/Box3D.h"
//#include "../Source/SampleGround.h"
#include "../Source/Camera.h"
#include "../Source/Player.h"
#include "../Source/ScoreViewer.h"
#include "../Source/ResultScene.h"
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

	Player* player = Instantiate<Player>();

	Camera* pCamera{ Instantiate<Camera>(player) };
	RectF rect{ 608,13,800,43 };
	int fontSize{ 36 };
	Instantiate<ScoreViewer>(rect,fontSize,TextAlignment::topLeft);
	CameraHandleInScene hCamera = RegisterCameraGameObject(pCamera);
	player->SetCamera(hCamera);
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
