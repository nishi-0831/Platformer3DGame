#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_internal.h"

#include "ImGui/ImGuizmo.h"
#include "ImGuiEditorCamera.h"
#include "Core/Game.h"
#include "Core/GameObject/GameObject.h"
#include "Components/Transform/Transform.h"
#include "Core/SceneSystem.h"
#include "Input/InputData.h"
#include "CameraSystem.h"
#include "GameTime.h"
#include "Components/Collider/ColliderCP.h"
#include "Editor/ImGuiUtil.h"
#include "EventManager.h"
#include "GameObjectSelectionEvent.h"
#include "MTImGui.h"
#include "ImGuiRenderer.h"
#include <cmath>
#include <algorithm>
#include "Debug.h"
#include <d3d11.h>
#include "Math/Mathf.h"
#include "Math/QuatToEuler.h"
#include "Screen.h"
#include "ImGuiEditor.h"
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
	, dragging_ { false }
	, rectFrameColor_ { IM_COL32(0, 0, 150, 255) }
	, rectFillColor_ { IM_COL32(0, 0, 100, 50) }
	, dragThresholdMovement_ { 1.0f }
	, selectionMode_ { SelectionMode::REPLACE }
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
				{
					mouseDownInWindow_ = false;
					dragging_		   = false;
					return;
				}

				DoTrack();
				if (InputUtil::GetMouseDown(MouseCode::LEFT))
				{
					if (ImGuizmo::IsViewManipulateHovered() == false && ImGuizmo::IsUsing() == false)
					{
						SelectGameObject();
					}
				}
				if (InputUtil::GetMouse(MouseCode::LEFT) && InputUtil::GetKey(KeyCode::LEFT_SHIFT) &&
					Game::System<ImGuiEditor>().GetOperation() == ImGuizmo::OPERATION::TRANSLATE && ImGuizmo::IsUsing())
				{
					SurfaceSnap();
				}

				ProcessDrag();
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
	PropertyDisplayRegistry::Instance().ShowProperty(&pCameraTransform_->position, "cameraPos");
	PropertyDisplayRegistry::Instance().ShowProperty(&pCameraTransform_->rotate, "cameraRot");
	PropertyDisplayRegistry::Instance().ShowProperty(&dragRect_, "dragRect");
	ImGui::InputFloat4("quat", pCameraTransform_->rotate.f);
	ImGui::InputFloat("AngleX", &polarAngleRad_);
	ImGui::InputFloat("AngleY", &azimuthalAngleRad_);
	ImGui::InputFloat("windowX", &windowPos_.x);
	ImGui::InputFloat("windowY", &windowPos_.y);
	ImGui::Checkbox("drag", &dragging_);

	const char* statName = ShowState(sCameraOperation_.Current());
	ImGui::LabelText("State", "%s", statName);
	Vector3 euler = QuatToEuler(pCameraTransform_->rotate);
	PropertyDisplayRegistry::Instance().ShowProperty(&euler, "euler");
}

void mtgb::ImGuiEditorCamera::Initialize() {}

void mtgb::ImGuiEditorCamera::SetCamera()
{
	Game::System<CameraSystem>().SetDrawCamera(hCamera_);
}

void mtgb::ImGuiEditorCamera::Update()
{
	if (ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))
	{
		sCameraOperation_.Update();
		CameraOperation operation;
		if (sCameraOperation_.TryGetNextState(operation))
		{
			sCameraOperation_.Change(operation);
		}
	}
	if (pCameraTransform_ == nullptr)
		return;
	DirectX::XMVECTOR forward		= DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	DirectX::XMVECTOR rotatedVector = DirectX::XMVector3Rotate(forward, pCameraTransform_->rotate);

	Mathf::SphericalCoord coord = Mathf::CartesianToSpherical(DirectX::XMVector4Normalize(rotatedVector));
	polarAngleRad_				= coord.theta;
	azimuthalAngleRad_			= coord.phi;
	polarAngleRad_				= std::clamp(polarAngleRad_, minPolarAngleRad_, maxPolarAngleRad_);
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

		MoveCameraOnTheSpot();
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

void mtgb::ImGuiEditorCamera::MoveCameraOnTheSpot()
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

void mtgb::ImGuiEditorCamera::SelectGameObject()
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

	Intersection::RaycastInfo info;
	EntityId entityId = Game::System<ColliderCP>().RayCastHitAll(origin, direction, distance, &info);
	if (entityId != INVALID_ENTITY)
	{
		// EntityがTransformコンポーネントを持っていない可能性があるのでTryGet
		Game::System<TransformCP>().TryGet(pTargetTransform_, entityId);
		mtgb::GameObjectSelectedEvent event { .entityIds = { entityId }, .selectionMode = SelectionMode::REPLACE };
		if (InputUtil::GetKey(KeyCode::LEFT_CONTROL))
		{
			event.selectionMode = SelectionMode::ADD;
		}
		Game::System<EventManager>().GetEvent<mtgb::GameObjectSelectedEvent>().Invoke(event);
		LOGIMGUI("EditorCamera:Selected");
	}
	else
	{
		pTargetTransform_ = nullptr;
		Game::System<EventManager>().GetEvent<mtgb::SelectionClearedEvent>().Invoke(SelectionClearedEvent {});
		LOGIMGUI("EditorCamera:No Select");
	}
}

void mtgb::ImGuiEditorCamera::ProcessDrag()
{
	windowPos_ = ImGui::FindWindowByName(windowName_.c_str())->WorkRect.Min;

	// マウスを押下した瞬間
	if (InputUtil::GetMouseDown(MouseCode::LEFT) && IsMouseInWindow(windowName_.c_str()))
	{
		mouseDownInWindow_ = true;

		// 選択範囲の始点を記録する
		ImVec2 mousePos = ImGui::GetMousePos();
		Vector2F mousePosInSceneView { mousePos.x - windowPos_.x, mousePos.y - windowPos_.y };
		dragRect_.point	 = mousePosInSceneView;
		dragRect_.size.x = 0.0f;
		dragRect_.size.y = 0.0f;
	}

	// マウスを押下している、ギズモを操作していない場合
	if (InputUtil::GetMouse(MouseCode::LEFT) && IsMouseInWindow(windowName_.c_str()) &&
		ImGuizmo::IsUsingAny() == false && mouseDownInWindow_)
	{
		ImVec2 mousePos = ImGui::GetMousePos();
		Vector2F mousePosInSceneView { mousePos.x - windowPos_.x, mousePos.y - windowPos_.y };
		// 押下した際の座標と現在の座標で矩形を作成
		dragRect_ = RectF::FromLine(dragRect_.point, mousePosInSceneView);

		float movementX = std::abs(dragRect_.size.x);
		float movementY = std::abs(dragRect_.size.y);
		// 矩形のサイズが閾値を超えたら、ドラッグ状態にする
		if (movementX > dragThresholdMovement_ && movementY > dragThresholdMovement_)
		{
			dragging_ = true;
		}
	}
	if (dragging_)
	{
		// 選択範囲を描画
		MTImGui::DirectShow(
			[this]()
			{
				Vector2F begin = dragRect_.GetBegin();
				Vector2F end   = dragRect_.GetEnd();

				ImVec2 minPoint(begin.x, begin.y);
				ImVec2 maxPoint(end.x, end.y);

				// 矩形の中身を描画
				ImGui::GetWindowDrawList()->AddRectFilled(minPoint + windowPos_, maxPoint + windowPos_, rectFillColor_);
				// 矩形の枠を描画
				ImGui::GetWindowDrawList()->AddRect(minPoint + windowPos_, maxPoint + windowPos_, rectFrameColor_);
			},
			"",
			ShowType::SCENE_VIEW
		);
	}
	if (InputUtil::GetMouseUp(MouseCode::LEFT) && mouseDownInWindow_)
	{
		if (dragging_)
		{
			RectSelect();
		}
		dragging_		   = false;
		mouseDownInWindow_ = false;
	}
}

void mtgb::ImGuiEditorCamera::RectSelect()
{
	using namespace DirectX;
	// REF: 透視投影行列の式 https: // marina.sys.wakayama-u.ac.jp/~tokoi/?date=20090907

	Vector2F windowPosVec2 = Vector2F(windowPos_.x, windowPos_.y);
	Vector2F begin		   = dragRect_.GetBegin();
	Vector2F end		   = dragRect_.GetEnd();
	const Vector2Int SCREEN_SIZE { Game::System<Screen>().GetSize() };
	float viewW = static_cast<float>(SCREEN_SIZE.x);
	float viewH = static_cast<float>(SCREEN_SIZE.y);

	// 選択範囲(スクリーン座標)をNDCに変換する
	float x0 = 2.0f * (begin.x / viewW) - 1.0f;
	float x1 = 2.0f * (end.x / viewW) - 1.0f;
	float y0 = 1.0f - 2.0f * (begin.y / viewH);
	float y1 = 1.0f - 2.0f * (end.y / viewH);

	CameraSystem& cameraSystem = Game::System<CameraSystem>();
	float nearZ				   = cameraSystem.GetNear();
	float farZ				   = cameraSystem.GetFar();
	float fov				   = XMConvertToRadians(cameraSystem.GetFov());
	float aspect			   = viewW / viewH;
	float tanHalfY			   = std::tanf(fov * 0.5f);
	float tanHalfX			   = tanHalfY * aspect;

	// 選択範囲の矩形を、ニアクリップ平面に射影した値を求める

	// 画面全体のニアクリップ平面の幅/高さを求める
	float halfWidth	 = nearZ * tanHalfX;
	float halfHeight = nearZ * tanHalfY;

	// 幅、高さを選択範囲にクリッピングする
	float left	 = halfWidth * x0;
	float right	 = halfWidth * x1;
	float top	 = halfHeight * y0;
	float bottom = halfHeight * y1;

	// 透視投影行列を作成
	XMMATRIX projMat = XMMatrixPerspectiveOffCenterLH(left, right, bottom, top, nearZ, farZ);
	// 視錐台作成
	BoundingFrustum frustum(projMat);
	// カメラの座標、姿勢に変換
	frustum.Transform(frustum, 1.0f, pCameraTransform_->rotate, pCameraTransform_->position);

	// 全ゲームオブジェクトと当たり判定を取る
	std::list<GameObject*> gameObjList;
	Game::System<SceneSystem>().GetActiveScene()->GetAllGameObjects(&gameObjList);
	std::vector<EntityId> containsEntityIds;
	for (auto obj : gameObjList)
	{
		// トランスフォーム取得
		EntityId id			  = obj->GetEntityId();
		Transform* pTransform = nullptr;
		Game::System<TransformCP>().TryGet(pTransform, id);
		if (pTransform == nullptr)
			continue;

		// ゲームオブジェクト自体の座標と判定をとる
		ContainmentType containmentType = frustum.Contains(pTransform->position);
		if (containmentType == ContainmentType::CONTAINS)
		{
			containsEntityIds.push_back(id);
		}
	}
	mtgb::GameObjectSelectedEvent event { .entityIds = containsEntityIds, .selectionMode = selectionMode_ };
	Game::System<EventManager>().GetEvent<mtgb::GameObjectSelectedEvent>().Invoke(event);
}

void mtgb::ImGuiEditorCamera::SurfaceSnap()
{
	auto selectedEntityIds = Game::System<ImGuiEditor>().GetSelectedEntityId();
	if (selectedEntityIds.size() != 1)
	{
		return;
	}
	EntityId selectedEntityId = selectedEntityIds[0];
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
	Vector3 direction = vec.Normalize();

	const CameraSystem& camera = Game::System<CameraSystem>();
	float distance			   = camera.GetFar() - camera.GetNear();

	Intersection::RaycastInfo info;
	EntityId entityId =
		Game::System<ColliderCP>()
			.RayCastHitAll(origin, direction, distance, &info, ColliderTag::GAME_OBJECT, selectedEntityId);
	if (entityId != INVALID_ENTITY)
	{
		Transform& selectedTransform = Game::System<TransformCP>().Get(selectedEntityId);
		Collider& selectedCollider	 = Game::System<ColliderCP>().Get(selectedEntityId);

		float pushDistance = 0.0f;
		Vector3 n		   = info.normal;

		ColliderType type = selectedCollider.colliderType_;
		// 球の場合
		if (type == ColliderType::TYPE_SPHERE)
		{
			float radius = selectedCollider.GetRadius();
			pushDistance = std::abs(n.x) * radius + std::abs(n.y) * radius + std::abs(n.z) * radius;
		}
		// AABB、OBBの場合
		else if (type == ColliderType::TYPE_AABB || type == ColliderType::TYPE_OBB)
		{
			Vector3 extents = selectedCollider.GetExtents();
			// OBBならば、extentsを回転させる
			if (type == ColliderType::TYPE_OBB)
			{
				Matrix4x4 rotMat;
				selectedTransform.GenerateWorldRotationMatrix(&rotMat);
				extents = extents * rotMat;
			}
			pushDistance = std::abs(n.x) * extents.x + std::abs(n.y) * extents.y + std::abs(n.z) * extents.z;
		}
		// 接触したコライダーの面の法線方向に、コライダーのサイズ分押し出す
		selectedTransform.position = info.point + info.normal * pushDistance;
		selectedTransform.Compute();
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