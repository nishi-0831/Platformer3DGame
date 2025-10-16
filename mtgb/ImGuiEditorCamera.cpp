#include "ImGuiEditorCamera.h"
#include "Game.h"
#include "GameObject.h"
#include "Transform.h"
#include "SceneSystem.h"
#include "InputData.h"
#include "CameraSystem.h"
mtgb::ImGuiEditorCamera::ImGuiEditorCamera()
	: orbitDistance_{10.0f}
	, moveSpeed_{10.0f}
	, orbitSpeed_{1.0f}
	, rotateSensitivity_{1.0f}
	, pCameraTransform_{nullptr}
	, angleX_{0.0f}
	, angleY_{0.0f}
{
	sCameraOperation_
		.OnAnyUpdate([this]
			{
				if (true /*何も押していない*/)
				{
					sCameraOperation_.Change(CameraOperation::None);
				}

				if (true /*特定のキー && 特定のキー押下中*/)
				{
					sCameraOperation_.Change(CameraOperation::Orbit);
				}
				if (true /*特定のキー押下中*/)
				{
					sCameraOperation_.Change(CameraOperation::Pan);
				}
			})
		.OnUpdate(CameraOperation::Translate, [this]
			{

			})
		.RegisterTransition(CameraOperation::None, CameraOperation::Translate, []() { return InputUtil::GetMouse(MouseCode::Middle); })
		.RegisterTransition(CameraOperation::None, CameraOperation::Orbit, []() {return InputUtil::GetKey(KeyCode::LeftMenu); })
		.RegisterTransition(CameraOperation::None, CameraOperation::Pan, []() {return InputUtil::GetMouse(MouseCode::Right); })
		//.RegisterAnyTransition(CameraOperation::None,[](){return InputUtil::G})
}

mtgb::ImGuiEditorCamera::~ImGuiEditorCamera()
{
}

void mtgb::ImGuiEditorCamera::Initialize()
{
	GameObject* pCamera = new GameObject(
		GameObjectBuilder()
		.SetPosition({ 0,0,0 })
		.SetName("Camera")
		.Build());
	Game::System<SceneSystem>().GetActiveScene()->RegisterGameObject(pCamera);

	pCameraTransform_ = &Game::System<TransformCP>().Get(pCamera->GetEntityId());
	hCamera_ = Game::System<CameraSystem>().RegisterDrawCamera(pCameraTransform_);
}

void mtgb::ImGuiEditorCamera::SetCamera()
{
	Game::System<CameraSystem>().SetDrawCamera(hCamera_);
}
