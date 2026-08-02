#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_internal.h"

#include "ImGui/ImGuizmo.h"
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
	const mtgb::Vector3 INIT_ANGLE { 0, 0, 0 };
} // namespace
mtgb::ImGuiEditorCamera::ImGuiEditorCamera()
	: ImGuiShowable { "EditorCamera", ShowType::EDITOR, INVALID_ENTITY, ImGuiShowable::Scope::GLOBAL }
	, hCamera_ { INVALID_ENTITY }
	, rotateSensitivity_ { 1.0f }
	, moveSpeed_ { 10.0f }
	, frameSelectedDistanceScale_ { 1.2f }
{
	distance_	= 10.0f;
	orbitSpeed_ = 1.0f;

	windowName_ = MTImGui::GetName(ShowType::SCENE_VIEW);

	// Dolly
	sCameraOperation_
		.OnUpdate(
			CameraOperation::DOLLY,
			[this]
			{
				DoDolly();
			}
		)
		.RegisterTransition(
			CameraOperation::DOLLY,
			CameraOperation::TRACK,
			[]()
			{
				return (InputUtil::GetMouse(MouseCode::MIDDLE) == false);
			}
		);

	// Orbit
	sCameraOperation_
		.OnUpdate(
			CameraOperation::ORBIT,
			[this]
			{
				DoOrbit();
			}
		)
		.RegisterTransition(
			CameraOperation::ORBIT,
			CameraOperation::TRACK,
			[]()
			{
				return (InputUtil::GetKey(KeyCode::LEFT_MENU) == false);
			}
		);

	// Pan
	sCameraOperation_
		.OnUpdate(
			CameraOperation::PAN,
			[this]
			{
				DoPan();
			}
		)
		.RegisterTransition(
			CameraOperation::PAN,
			CameraOperation::TRACK,
			[]()
			{
				return (InputUtil::GetMouse(MouseCode::RIGHT) == false);
			}
		);

	// Track
	sCameraOperation_
		.OnUpdate(
			CameraOperation::TRACK,
			[this]
			{
				if (IsMouseInWindow(windowName_.c_str()) == false)
					return;

				DoTrack();
				if (InputUtil::GetMouseDown(MouseCode::LEFT))
				{
					if ((!ImGuizmo::IsViewManipulateHovered()))

						if (!ImGuizmo::IsUsing())
						{
							SelectTransform();
						}
				}
			}
		)
		.RegisterTransition(
			CameraOperation::TRACK,
			CameraOperation::PAN,
			[this]()
			{
				return InputUtil::GetMouse(MouseCode::RIGHT) && IsMouseInWindow(windowName_.c_str());
			}
		)
		.RegisterTransition(
			CameraOperation::TRACK,
			CameraOperation::ORBIT,
			[this]()
			{
				return InputUtil::GetKey(KeyCode::LEFT_MENU) && IsMouseInWindow(windowName_.c_str());
			}
		)
		.RegisterTransition(
			CameraOperation::TRACK,
			CameraOperation::DOLLY,
			[this]()
			{
				return InputUtil::GetMouse(MouseCode::MIDDLE) && IsMouseInWindow(windowName_.c_str());
			}
		);
}

mtgb::ImGuiEditorCamera::~ImGuiEditorCamera() {}

void mtgb::ImGuiEditorCamera::ShowImGui()
{
	ImVec2 mousePos	 = ImGui::GetMousePos();
	ImVec2 windowPos = ImGui::GetWindowPos();

	ImVec2 localPos = ImVec2(mousePos.x - windowPos.x, mousePos.y - windowPos.y);

	PropertyDisplayRegistry::Instance().ShowProperty(&pCameraTransform_->position, "cameraPos");
	PropertyDisplayRegistry::Instance().ShowProperty(&pCameraTransform_->rotate, "cameraRot");
	ImGui::InputFloat4("quat", pCameraTransform_->rotate.f);
	ImGui::InputFloat("AngleX", &polarAngleRad_);
	ImGui::InputFloat("AngleY", &azimuthalAngleRad_);
	const char* statName = ShowState(sCameraOperation_.Current());
	ImGui::LabelText("State", "%s", statName);
}

void mtgb::ImGuiEditorCamera::Initialize() {}

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
	// カメラに使うGameObject作成
	GameObject* pCamera		= new GameObject(GameObjectBuilder()
											 .SetPosition({ 0, 0, 0 })
											 .SetRotate(Quaternion::Euler(INIT_ANGLE))
											 .SetName("EditorCamera")
											 .Build());
	pCamera->isInspectable_ = false;
	// シーンに登録
	Game::System<SceneSystem>().GetActiveScene()->RegisterGameObject(pCamera);

	// Transformをアタッチ
	pCameraTransform_ = &Game::System<TransformCP>().Get(pCamera->GetEntityId());
	// Transformをカメラとして登録
	hCamera_ = Game::System<CameraSystem>().RegisterDrawCamera(pCameraTransform_);

	// 初期角度を設定
	polarAngleRad_	   = DirectX::XMConvertToRadians(INIT_ANGLE.x + 90.0f);
	azimuthalAngleRad_ = DirectX::XMConvertToRadians(INIT_ANGLE.y + 90.0f);
}

void mtgb::ImGuiEditorCamera::FrameSelected(EntityId _entityId)
{
	if (_entityId == INVALID_ENTITY)
		return;
	Collider* pCollider = nullptr;
	Game::System<ColliderCP>().TryGet(pCollider, _entityId);

	if (pCollider == nullptr)
		return;

	float fovRad = DirectX::XMConvertToRadians(Game::System<CameraSystem>().GetFov());

	float radius = 0.0f;
	if (pCollider->colliderType_ == ColliderType::TYPE_SPHERE)
	{
		radius = pCollider->GetRadius();
	}
	if (pCollider->colliderType_ == ColliderType::TYPE_AABB || pCollider->colliderType_ == ColliderType::TYPE_OBB)
	{
		Vector3 extents = pCollider->GetExtents();
		// 一番大きな値を半径とする
		radius = (std::max)({ extents.x, extents.y, extents.z });
	}
	// 対象を画面に収めるために必要な距離を計算
	float baseDistance = radius / std::sinf(fovRad / 2.0f);
	// 倍率をかけた最終的なカメラ距離
	float distance = baseDistance * frameSelectedDistanceScale_;

	// カメラの位置を設定
	Vector3 center				= pCollider->GetCenter() + Game::System<TransformCP>().Get(_entityId).position;
	pCameraTransform_->position = center + pCameraTransform_->Back() * distance;
}

void mtgb::ImGuiEditorCamera::DoDolly()
{
	Vector3 mouseMove = InputUtil::GetMouseMove();
	if (mouseMove.Size() != 0)
	{
		// カメラの右、上ベクトル
		Vector3 right = pCameraTransform_->Right();
		Vector3 up	  = pCameraTransform_->Up();

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
		azimuthalAngleRad_ -= mouseMove.x * rotateSensitivity_ * Time::DeltaTimeF(); // 水平角度

		polarAngleRad_ += mouseMove.y * rotateSensitivity_ * Time::DeltaTimeF(); // 鉛直角度

		// 鉛直角度を制限
		polarAngleRad_ = std::clamp(polarAngleRad_, minPolarAngleRad_, maxPolarAngleRad_);

		MoveCameraSphericalOnTheSpot();
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

void mtgb::ImGuiEditorCamera::MoveCameraSphericalOnTheSpot()
{
	// ref:https://ja.wikipedia.org/wiki/%E7%90%83%E9%9D%A2%E5%BA%A7%E6%A8%99%E7%B3%BB

	// θ (polar angle) : 鉛直方向
	float theta = polarAngleRad_;

	// φ (azimuthal angle): 水平方向
	float phi = azimuthalAngleRad_;

	// 回転中心からのオフセット
	Vector3 offset;

	// 変換
	offset.x = sinf(theta) * cos(phi);
	offset.y = cos(theta);
	offset.z = sin(theta) * sin(phi);

	// その場回転の時はoffsetの方向を向く
	pCameraTransform_->rotate = Quaternion::LookRotation(offset, Vector3::Up());
}

void mtgb::ImGuiEditorCamera::SelectTransform()
{
	Vector3 origin, end, vec;
	Matrix4x4 proj, view;
	Game::System<CameraSystem>().GetProjMatrix(&proj);
	Game::System<CameraSystem>().GetViewMatrix(&view);

	ImGuiWindow* window = ImGui::FindWindowByName(windowName_.c_str());

	if (window == nullptr)
		return;

	ImRect workRect = window->WorkRect;
	ImVec2 workPos	= workRect.Min;
	ImGuiUtil::GetMouseRay(
		origin,
		end,
		proj,
		view,
		Game::System<ImGuiRenderer>().GetViewport(),
		{ workPos.x, workPos.y }
	);

	vec = end - origin;

	// vec.Normalize()の結果を別変数に保存して、元の長さを保持
	Vector3 direction = vec.Normalize(); // これで正規化されたベクトルが返される

	const CameraSystem& camera = Game::System<CameraSystem>();
	float distance			   = camera.GetFar() - camera.GetNear(); // 元の長さを計算

	EntityId entityId = Game::System<ColliderCP>().RayCastHitAll(origin, direction, &distance);
	if (entityId != INVALID_ENTITY)
	{
		// EntityがTransformコンポーネントを持っていない可能性があるのでTryGet
		Game::System<TransformCP>().TryGet(pTargetTransform_, entityId);

		mtgb::GameObjectSelectedEvent event { .entityId = entityId };
		Game::System<EventManager>().GetEvent<mtgb::GameObjectSelectedEvent>().Invoke(event);
		LOGIMGUI("EditorCamera:Selected");
	}
	else
	{
		mtgb::GameObjectDeselectedEvent event;
		if (pTargetTransform_ != nullptr)
		{
			event			  = { .entityId = pTargetTransform_->GetEntityId() };
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
		case CameraOperation::TRACK :
			return "Track";
		case CameraOperation::DOLLY :
			return "Dolly";
		case CameraOperation::PAN :
			return "Pan";
		case CameraOperation::ORBIT :
			return "Orbit";
		default :
			return "Unknown";
	}
}