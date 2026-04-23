#include "stdafx.h"
#include <mtgb.h>
#include "TitleScene.h"
#include "Scenes/SampleScene.h"
#include "../Source/SkySphere.h"
#include "../Source/SeaPlane.h"

namespace
{
	// 118,90 , 565,100
	ImageHandle hTitleImage;
	ImageHandle hBackgroundImage;
	FBXModelHandle hModel;
	RectF draw { 118, 90, 565, 100 };
	RectF textDrawRect { 118, 300, 565, 100 };
} // namespace
TitleScene::TitleScene() {}

TitleScene::~TitleScene() {}

void TitleScene::Initialize()
{
	Game::System<ImGuiEditorCamera>().CreateCamera();
	GameObject* pCamera =
		new GameObject(GameObjectBuilder().SetPosition({ 0, 50, -80 }).SetName("SceneCamera").Build());
	Game::System<SceneSystem>().GetActiveScene()->RegisterGameObject(pCamera);
	hTitleImage		 = Image::Load("Image/TitleImage.png");
	hBackgroundImage = Image::Load("Image/Black.png");

	CameraHandleInScene hCamera = RegisterCameraGameObject(pCamera);
	WinCtxRes::Get<CameraResource>(WindowContext::FIRST).SetHCamera(hCamera);

	GameObject* pTitleRuin =
		new GameObject(GameObjectBuilder()
						   .SetPosition({ -10, 0, 0 })
						   .SetRotate(Quaternion::AngleAxis(DirectX::XMConvertToRadians(90), Vector3::Up()))
						   .SetName("TitleRuin")
						   .Build());
	Game::System<SceneSystem>().GetActiveScene()->RegisterGameObject(pTitleRuin);
	MeshRenderer* pMeshRenderer = pTitleRuin->Component<MeshRenderer>();
	hModel						= Fbx::Load("Model/TitleRuin2.fbx");
	pMeshRenderer->SetMesh(hModel);

	Instantiate<mtgb::SkySphere>();
	Transform* pTransform  = Instantiate<SeaPlane>()->Component<Transform>();
	pTransform->position.y = -70.0f;
	pTransform->scale	   = Vector3 { 1800, 0, 1800 };
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
	Draw::Image(hBackgroundImage, textDrawRect, mtgb::UIParams {}, mtgb::Color(0, 0, 0, 127));
	Draw::ImmediateText("push P Key to start game...", textDrawRect);
}

void TitleScene::End() {}
