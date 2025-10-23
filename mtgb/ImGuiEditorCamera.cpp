#include"../ImGui/imgui_impl_win32.h"
#include "../ImGui/imgui_internal.h"

#include "../ImGui/ImGuizmo.h"
#include "ImGuiEditorCamera.h"
#include "Game.h"
#include "GameObject.h"
#include "Transform.h"
#include "SceneSystem.h"
#include "InputData.h"
#include "CameraSystem.h"
#include "GameTime.h"
#include "ColliderCP.h"
#include "ImGuiUtil.h"
#include "EventManager.h"
#include "GameObjectSelectionEvent.h"
#include "MTImGui.h"
#include "ImGuiRenderer.h"
#include <cmath>
#include <algorithm>
#include "Debug.h"
#include <d3d11.h>
using namespace mtgb::ImGuiUtil;

const char* ShowState(mtgb::CameraOperation _cameraOperation);

namespace
{
	const mtgb::Vector3 INIT_ANGLE{ 0,0,0 };
}
mtgb::ImGuiEditorCamera::ImGuiEditorCamera()
	: orbitDistance_{10.0f}
	, moveSpeed_{10.0f}
	, orbitSpeed_{1.0f}
	, rotateSensitivity_{1.0f}
	, pCameraTransform_{nullptr}
	, pTargetTransform_{nullptr}
	, angleX_{0.0f}
	, angleY_{0.0f}
	, hCamera_{INVALID_ENTITY}
{
	windowName_ = MTImGui::Instance().GetName(ShowType::SceneView);

	// Dolly
	sCameraOperation_
		.OnUpdate(CameraOperation::Dolly, [this]
			{
				DoDolly();
			})
		.RegisterTransition(CameraOperation::Dolly, CameraOperation::Track, []() { return (InputUtil::GetMouse(MouseCode::Middle) == false); });

	// Orbit
	sCameraOperation_
		.OnUpdate(CameraOperation::Orbit, [this]
			{
				DoOrbit();
			})
		.RegisterTransition(CameraOperation::Orbit, CameraOperation::Track, []() {return (InputUtil::GetKey(KeyCode::LeftMenu) == false); });

	// Pan
	sCameraOperation_
		.OnUpdate(CameraOperation::Pan, [this]
			{
				DoPan();
			})
		.RegisterTransition(CameraOperation::Pan, CameraOperation::Track, []() {return (InputUtil::GetMouse(MouseCode::Right) == false); });

	// Track
	sCameraOperation_
		.OnUpdate(CameraOperation::Track, [this]
			{
				DoTrack();
				if (InputUtil::GetMouseDown(MouseCode::Left))
				{
					if ((!ImGuizmo::IsViewManipulateHovered()))
						
						if (!ImGuizmo::IsUsing())
						{
							if (IsMouseInWindow(windowName_.c_str()))
							{
								SelectTransform();
							}
						}
				}
			})
		.RegisterTransition(CameraOperation::Track, CameraOperation::Pan,
			[this]()
			{
				return InputUtil::GetMouse(MouseCode::Right) && IsMouseInWindow(windowName_.c_str());
			})
		.RegisterTransition(CameraOperation::Track, CameraOperation::Orbit,
			[this]()
			{
				return InputUtil::GetKey(KeyCode::LeftMenu) && IsMouseInWindow(windowName_.c_str());
			})
		.RegisterTransition(CameraOperation::Track, CameraOperation::Dolly,
			[this]()
			{
				return InputUtil::GetMouse(MouseCode::Middle) && IsMouseInWindow(windowName_.c_str());
			});
		
}

mtgb::ImGuiEditorCamera::~ImGuiEditorCamera()
{
}

void mtgb::ImGuiEditorCamera::ShowImGui()
{
	ImVec2 mousePos = ImGui::GetMousePos();
	ImVec2 windowPos = ImGui::GetWindowPos();
	
	ImVec2 localPos = ImVec2(mousePos.x - windowPos.x, mousePos.y - windowPos.y);

	TypeRegistry::Instance().CallFunc(&pCameraTransform_->position, "cameraPos");
	TypeRegistry::Instance().CallFunc(&pCameraTransform_->rotate, "cameraRot");
	ImGui::InputFloat4("quat", pCameraTransform_->rotate.f);
	ImGui::InputFloat("AngleX", &angleX_);
	ImGui::InputFloat("AngleY", &angleY_);
	const char* statName = ShowState(sCameraOperation_.Current());
	ImGui::LabelText("State", "%s", statName);
}

void mtgb::ImGuiEditorCamera::Initialize()
{

	
}

void mtgb::ImGuiEditorCamera::SetCamera()
{
	Game::System<CameraSystem>().SetDrawCamera(hCamera_);
}

void mtgb::ImGuiEditorCamera::Update()
{
	sCameraOperation_.Update();

	CameraOperation operation;
	if (sCameraOperation_.TryGetNextState(operation))
	{
		sCameraOperation_.Change(operation);
	}
}

void mtgb::ImGuiEditorCamera::CreateCamera()
{
	GameObject* pCamera = new GameObject(
		GameObjectBuilder()
		.SetPosition({ 0,0,0 })
		.SetRotate(Quaternion::Euler(INIT_ANGLE))
		.SetName("Camera")
		.Build());
	Game::System<SceneSystem>().GetActiveScene()->RegisterGameObject(pCamera);

	pCameraTransform_ = &Game::System<TransformCP>().Get(pCamera->GetEntityId());
	hCamera_ = Game::System<CameraSystem>().RegisterDrawCamera(pCameraTransform_);

	angleX_ = DirectX::XMConvertToRadians(INIT_ANGLE.x + 90.0f);
	angleY_ = DirectX::XMConvertToRadians(INIT_ANGLE.y + 90.0f);
}


void mtgb::ImGuiEditorCamera::DoDolly()
{
	Vector3 mouseMove = InputUtil::GetMouseMove();
	if (mouseMove.Size() != 0)
	{
		// カメラの右、上ベクトル
		Vector3 right = pCameraTransform_->Right();
		Vector3 up = pCameraTransform_->Up();

		// 移動量を合成
		Vector3 move = right * -mouseMove.x + up * mouseMove.y;

		pCameraTransform_->position += move * moveSpeed_ * Time::DeltaTimeF();
	}
}

void mtgb::ImGuiEditorCamera::DoPan()
{
	Vector3 mouseMove = InputUtil::GetMouseMove();
	if (mouseMove.Size() != 0)
	{
		// マウス移動量を角度に変換
		
		
		angleY_ -= mouseMove.x * rotateSensitivity_ * Time::DeltaTimeF(); // 水平角度
		
		angleX_ += mouseMove.y * rotateSensitivity_ * Time::DeltaTimeF(); // 鉛直角度
	
		// 鉛直角度を制限
		angleX_ = std::clamp(angleX_, DirectX::XMConvertToRadians(0.1f), DirectX::XMConvertToRadians(179.0f));

		MoveCameraSphericalOnTheSpot();
	}
}

void mtgb::ImGuiEditorCamera::DoOrbit()
{
	Vector3 mouseMove = InputUtil::GetMouseMove();
	if (mouseMove.Size() != 0)
	{
		angleY_ -= mouseMove.x * orbitSpeed_ * Time::DeltaTimeF();
		angleX_ += mouseMove.y * orbitSpeed_ * Time::DeltaTimeF();

		// 鉛直角度を制限
		angleX_ = std::clamp(angleX_, DirectX::XMConvertToRadians(0.1f), DirectX::XMConvertToRadians(179.0f));
		MoveCameraSpherical(orbitDistance_);
	}
}

void mtgb::ImGuiEditorCamera::DoTrack()
{
	Vector3 mouseMove = InputUtil::GetMouseMove();
	if (mouseMove.Size() != 0)
	{
		Vector3 forward = pCameraTransform_->Forward();

		Vector3 move = forward * mouseMove.z;

		pCameraTransform_->position += move * moveSpeed_ * Time::DeltaTimeF();
	}
}

void mtgb::ImGuiEditorCamera::MoveCameraSpherical(float _distance)
{
	// ref:https://ja.wikipedia.org/wiki/%E7%90%83%E9%9D%A2%E5%BA%A7%E6%A8%99%E7%B3%BB

	// 現在のカメラ位置から回転中心を計算
	Vector3 center = Vector3::Zero();
	// ターゲットを中心に回転
	if (pTargetTransform_)
	{
		Vector3 toTarget = pTargetTransform_->position - pCameraTransform_->position;
		center = pTargetTransform_->position + (Vector3::Normalize(-toTarget) * _distance);
	}
	// ターゲットがいない場合は正面を向いて回転
	else
	{
		center = pCameraTransform_->position + (pCameraTransform_->Forward() * _distance);
	} 

	// θ (polar angle) : 鉛直方向
	float theta = angleX_;

	// φ (azimuthal angle): 水平方向
	float phi = angleY_;

	// 回転中心からのオフセット
	Vector3 offset;

	// 回転中心の方向を向く
	
	// 変換
	offset.x = _distance * sinf(theta) * cos(phi);
	offset.y = -_distance * cos(theta);
	offset.z = -_distance * sin(theta) * sin(phi);

	// 位置を反映
	pCameraTransform_->position = center + offset;

	Vector3 lookDir = center - pCameraTransform_->position;
		
	pCameraTransform_->rotate = Quaternion::LookRotation(lookDir.Normalize(), Vector3::Up());
	
}

void mtgb::ImGuiEditorCamera::MoveCameraSphericalOnTheSpot()
{
	// ref:https://ja.wikipedia.org/wiki/%E7%90%83%E9%9D%A2%E5%BA%A7%E6%A8%99%E7%B3%BB
	
	// θ (polar angle) : 鉛直方向
	float theta = angleX_;

	// φ (azimuthal angle): 水平方向
	float phi = angleY_;

	// 回転中心からのオフセット
	Vector3 offset;

	// 変換
	offset.x = sinf(theta) * cos(phi);
	offset.y = cos(theta);
	offset.z = sin(theta) * sin(phi);

	// その場回転の時はoffsetの方向を向く
	pCameraTransform_->rotate = Quaternion::LookRotation(offset, Vector3::Up());
}

void mtgb::ImGuiEditorCamera::FollowTarget()
{
	if (pTargetTransform_)
	{
		if (!followTarget_) return;

		if (adjustTargetDirection_)
		{
			pCameraTransform_->rotate = pTargetTransform_->rotate;
			pCameraTransform_->position = pTargetTransform_->position + (pTargetTransform_->Back() * followDistance_);
		}
		else
		{
			MoveCameraSpherical(followDistance_);
		}
	}

}

void mtgb::ImGuiEditorCamera::SelectTransform()
{
	Vector3 origin, end, vec;
	Matrix4x4 proj, view;
	Game::System<CameraSystem>().GetProjMatrix(&proj);
	Game::System<CameraSystem>().GetViewMatrix(&view);
	
	ImGuiWindow* window = ImGui::FindWindowByName(windowName_.c_str());

	if (window == nullptr) return;

	ImRect workRect = window->WorkRect;
	ImVec2 workPos = workRect.Min;
	ImGuiUtil::GetMouseRay(origin, end, proj, view, Game::System<ImGuiRenderer>().GetViewport(), {workPos.x,workPos.y});

	vec = end - origin;

	// vec.Normalize()の結果を別変数に保存して、元の長さを保持
	Vector3 direction = vec.Normalize();  // これで正規化されたベクトルが返される

	const CameraSystem& camera = Game::System<CameraSystem>();
	float distance = camera.GetFar() - camera.GetNear();          // 元の長さを計算

	EntityId entityId = Game::System<ColliderCP>().RayCastHitAll(origin, direction, distance);
	if (entityId != INVALID_ENTITY)
	{
		// EntityがTransformコンポーネントを持っていない可能性があるのでTryGet
		Game::System<TransformCP>().TryGet(pTargetTransform_, entityId);
		
		mtgb::GameObjectSelectedEvent event{ .entityId = entityId };
		Game::System<EventManager>().GetEvent<mtgb::GameObjectSelectedEvent>().Invoke(event);
		LOGIMGUI("EditorCamera:Selected");

	}
	else
	{
		mtgb::GameObjectDeselectedEvent event;
		if (pTargetTransform_ != nullptr)
		{
			event = { .entityId = pTargetTransform_->GetEntityId()};
			pTargetTransform_ = nullptr;
		}
		else
		{
			event = { .entityId = INVALID_ENTITY };
		}

		Game::System<EventManager>().GetEvent<mtgb::GameObjectDeselectedEvent>().Invoke(event);
		LOGIMGUI("EditorCamera:No Select");

	}
}

const char* ShowState(mtgb::CameraOperation _cameraOperation)
{
	using mtgb::CameraOperation;

	switch (_cameraOperation)
	{
	case CameraOperation::Track:
		return "Track";
	case CameraOperation::Dolly:
		return "Dolly";
	case CameraOperation::Pan:
		return "Pan";
	case CameraOperation::Orbit:
		return "Orbit";
	default:
		return "Unknown";
	}
}