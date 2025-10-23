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
		// �J�����̉E�A��x�N�g��
		Vector3 right = pCameraTransform_->Right();
		Vector3 up = pCameraTransform_->Up();

		// �ړ��ʂ�����
		Vector3 move = right * -mouseMove.x + up * mouseMove.y;

		pCameraTransform_->position += move * moveSpeed_ * Time::DeltaTimeF();
	}
}

void mtgb::ImGuiEditorCamera::DoPan()
{
	Vector3 mouseMove = InputUtil::GetMouseMove();
	if (mouseMove.Size() != 0)
	{
		// �}�E�X�ړ��ʂ��p�x�ɕϊ�
		
		
		angleY_ -= mouseMove.x * rotateSensitivity_ * Time::DeltaTimeF(); // �����p�x
		
		angleX_ += mouseMove.y * rotateSensitivity_ * Time::DeltaTimeF(); // �����p�x
	
		// �����p�x�𐧌�
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

		// �����p�x�𐧌�
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

	// ���݂̃J�����ʒu�����]���S���v�Z
	Vector3 center = Vector3::Zero();
	// �^�[�Q�b�g�𒆐S�ɉ�]
	if (pTargetTransform_)
	{
		Vector3 toTarget = pTargetTransform_->position - pCameraTransform_->position;
		center = pTargetTransform_->position + (Vector3::Normalize(-toTarget) * _distance);
	}
	// �^�[�Q�b�g�����Ȃ��ꍇ�͐��ʂ������ĉ�]
	else
	{
		center = pCameraTransform_->position + (pCameraTransform_->Forward() * _distance);
	} 

	// �� (polar angle) : ��������
	float theta = angleX_;

	// �� (azimuthal angle): ��������
	float phi = angleY_;

	// ��]���S����̃I�t�Z�b�g
	Vector3 offset;

	// ��]���S�̕���������
	
	// �ϊ�
	offset.x = _distance * sinf(theta) * cos(phi);
	offset.y = -_distance * cos(theta);
	offset.z = -_distance * sin(theta) * sin(phi);

	// �ʒu�𔽉f
	pCameraTransform_->position = center + offset;

	Vector3 lookDir = center - pCameraTransform_->position;
		
	pCameraTransform_->rotate = Quaternion::LookRotation(lookDir.Normalize(), Vector3::Up());
	
}

void mtgb::ImGuiEditorCamera::MoveCameraSphericalOnTheSpot()
{
	// ref:https://ja.wikipedia.org/wiki/%E7%90%83%E9%9D%A2%E5%BA%A7%E6%A8%99%E7%B3%BB
	
	// �� (polar angle) : ��������
	float theta = angleX_;

	// �� (azimuthal angle): ��������
	float phi = angleY_;

	// ��]���S����̃I�t�Z�b�g
	Vector3 offset;

	// �ϊ�
	offset.x = sinf(theta) * cos(phi);
	offset.y = cos(theta);
	offset.z = sin(theta) * sin(phi);

	// ���̏��]�̎���offset�̕���������
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

	// vec.Normalize()�̌��ʂ�ʕϐ��ɕۑ����āA���̒�����ێ�
	Vector3 direction = vec.Normalize();  // ����Ő��K�����ꂽ�x�N�g�����Ԃ����

	const CameraSystem& camera = Game::System<CameraSystem>();
	float distance = camera.GetFar() - camera.GetNear();          // ���̒������v�Z

	EntityId entityId = Game::System<ColliderCP>().RayCastHitAll(origin, direction, distance);
	if (entityId != INVALID_ENTITY)
	{
		// Entity��Transform�R���|�[�l���g�������Ă��Ȃ��\��������̂�TryGet
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