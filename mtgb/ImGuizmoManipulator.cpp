#include "ImGuizmoManipulator.h"
#include "GameObject.h"
#include "Vector3.h"
#include "Transform.h"
#include "Game.h"
#include "ISystem.h"
//#include "../ImGui/imgui.h"
//#include "../ImGui\ImGuizmo.h"

#include "../ImGui\imgui_internal.h"
#include "../ImGui/imgui_impl_win32.h"
#include "../ImGui/imgui_impl_dx11.h"
#include "CameraSystem.h"
#include "InputData.h"
#include "WindowContextUtil.h"
#include "GameTime.h"
#include "QuatToEuler.h"
#include "ImGuiRenderer.h"
#include "MTImGui.h"
#include <cmath>
#include <algorithm>
#include "SceneSystem.h"
namespace
{
	
}

void mtgb::ImGuizmoManipulator::DrawTransformGuizmo()
{
	if (!pTargetTransform_)
	{
		return;
	}
	uintptr_t ptrId = reinterpret_cast<uintptr_t>(pTargetTransform_);

	Calculate();

	ImGui::PushID(&ptrId);



	ImVec2 pos = ImGui::GetWindowPos();
	float windowWidth = (float)ImGui::GetWindowWidth();
	float windowHeight = (float)ImGui::GetWindowHeight();

	//�M�Y���\��
	float tabBarHeight = ImGui::GetCurrentWindow()->TitleBarHeight;
	ImGuizmo::SetRect(pos.x, pos.y + tabBarHeight, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);




	if (ImGuizmo::Manipulate(viewMat_, projMat_, operation_, mode_, worldMat_))
	{
		//�ҏW���ꂽworldMat����position,rotation,scale�ɕ���
		DirectX::XMMATRIX mat = DirectX::XMMATRIX(
			worldMat_[0], worldMat_[1], worldMat_[2], worldMat_[3],
			worldMat_[4], worldMat_[5], worldMat_[6], worldMat_[7],
			worldMat_[8], worldMat_[9], worldMat_[10], worldMat_[11],
			worldMat_[12], worldMat_[13], worldMat_[14], worldMat_[15]
		);

		DirectX::XMVECTOR scale, trans;
		bool result = DirectX::XMMatrixDecompose(&scale, &pTargetTransform_->rotate.v, &trans, mat);
		massert(result
			&& "XMMatrixDecompose�Ɏ��s @MTImGui::DrawTransformGuizmo");

		DirectX::XMStoreFloat3(&pTargetTransform_->position, trans);
		DirectX::XMStoreFloat3(&pTargetTransform_->scale, scale);

	}
	ImGui::PopID();
}

std::optional<ImVec2> mtgb::ImGuizmoManipulator::WorldToImGui(const Vector3& _vec)
{
	Game::System<CameraSystem>().GetViewMatrix(&viewMatrix4x4_);
	Game::System<CameraSystem>().GetProjMatrix(&projMatrix4x4_);

	const D3D11_VIEWPORT& viewport = Game::System<ImGuiRenderer>().GetViewport();
	ImVec2 windowPos = ImGui::GetWindowPos();
	float tabBarHeight = ImGui::GetCurrentWindow()->TitleBarHeight;


	Vector3 screenVec = DirectX::XMVector3Project(
		_vec,
		viewport.TopLeftX,
		viewport.TopLeftY,
		viewport.Width,
		viewport.Height,
		viewport.MinDepth,
		viewport.MaxDepth,
		projMatrix4x4_,
		viewMatrix4x4_,
		DirectX::XMMatrixIdentity());

	if (screenVec.z < 0.0f || screenVec.z > 1.0f)
		return std::nullopt;
	//return ImVec2(screenVec.x, screenVec.y );
	return ImVec2(screenVec.x + windowPos.x, screenVec.y + windowPos.y + tabBarHeight);
}

void mtgb::ImGuizmoManipulator::Calculate()
{
	//float[16]�̔z����쐬
	pTargetTransform_->GenerateWorldMatrix(&worldMatrix4x4);
	Game::System<mtgb::CameraSystem>().GetViewMatrix(&viewMatrix4x4_);
	Game::System<mtgb::CameraSystem>().GetProjMatrix(&projMatrix4x4_);


	//���[���h�s��
	DirectX::XMStoreFloat4x4(&float4x4_, worldMatrix4x4);
	memcpy(worldMat_, &float4x4_, sizeof(worldMat_));

	//�r���[�s��
	DirectX::XMStoreFloat4x4(&float4x4_, viewMatrix4x4_);
	memcpy(viewMat_, &float4x4_, sizeof(viewMat_));

	//�v���W�F�N�V�����s��
	DirectX::XMStoreFloat4x4(&float4x4_, projMatrix4x4_);
	memcpy(projMat_, &float4x4_, sizeof(projMat_));
}

void mtgb::ImGuizmoManipulator::SpinCamera(float _distance)
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
	float theta = spinAngleX_;

	// �� (azimuthal angle): ��������
	float phi = spinAngleY_;

	// ��]���S����̃I�t�Z�b�g
	Vector3 offset;
	
	// ��]���S�̕���������
	Vector3 lookDir = Vector3::Zero();
	if (_distance <= std::numeric_limits<float>::epsilon())
	{
		// �ϊ�
		offset.x = sinf(theta) * cos(phi);
		offset.y = cos(theta);
		offset.z = sin(theta) * sin(phi);
	}
	else
	{
		// �ϊ�
		offset.x = _distance * sinf(theta) * cos(phi);
		offset.y = -_distance * cos(theta);
		offset.z = -_distance * sin(theta) * sin(phi);

		// �ʒu�𔽉f
		pCameraTransform_->position =  center + offset;

		lookDir = center - pCameraTransform_->position;
	}

	if (lookDir.Size() == 0.0f)
	{
		// ���̏��]�̎���offset�̕���������
		pCameraTransform_->rotate = Quaternion::LookRotation(offset, Vector3::Up());
	}
	else
	{
		pCameraTransform_->rotate = Quaternion::LookRotation(lookDir.Normalize(), Vector3::Up());
	}
}

void mtgb::ImGuizmoManipulator::InitializeSpinAnglesFromCurrentPosition()
{
	// ref:https://ja.wikipedia.org/wiki/%E7%90%83%E9%9D%A2%E5%BA%A7%E6%A8%99%E7%B3%BB
	// ���s�������W���狅�ʍ��W�ւ̕ϊ�
	
	// ���݂̃J�����ʒu�����]���S���v�Z
	Vector3 center = Vector3::Zero();
	//// �^�[�Q�b�g�𒆐S�ɉ�]
	//if (pTargetTransform_)
	//{
	//	Vector3 toTarget = pTargetTransform_->position - pCameraTransform_->position;
	//	center = pTargetTransform_->position + (Vector3::Normalize(-toTarget) * spinDistance_);
	//}
	//// �^�[�Q�b�g�����Ȃ��ꍇ�͐��ʂ������ĉ�]
	//else
	//{
		center = pCameraTransform_->position + (pCameraTransform_->Forward() * spinDistance_);
	
	// ��]���S����̃I�t�Z�b�g
	Vector3 offset = pCameraTransform_->position - center;
	
	// ���ʍ��W�n�̊p�x���t�Z
	float r = offset.Size();
	if (r > 0.001f) // �[�����Z�������
	{
		// �� (polar angle): ��������
		spinAngleX_ = acosf(-offset.y / r);
		
		// �� (azimuthal angle): ��������
		spinAngleY_ = atan2f(-offset.z, offset.x);
		
		// �������X�V
		spinDistance_ = r;
	}
	else
	{
		// �J���������S�ɂ���ꍇ�̃f�t�H���g�l
		spinAngleX_ = DirectX::XMConvertToRadians(90.0f); // ����
		spinAngleY_ = 0.0f;
	}
}

mtgb::ImGuizmoManipulator::ImGuizmoManipulator()
	:ImGuiShowable("Manipulater", ShowType::SceneView)
	, operation_{ ImGuizmo::TRANSLATE }
	, mode_{ ImGuizmo::LOCAL }
	, spinDistance_{5.0f}
	, moveSpeed_{10.0f}
	, spinSpeed_{1.0f}
	, rotateSensitivity_{1.0f}
	, cameraOperation_{ CameraOperation::None }
	, followTarget_{true}
	, followDistance_{10.0f}
{
}

mtgb::ImGuizmoManipulator::~ImGuizmoManipulator()
{	
}

void mtgb::ImGuizmoManipulator::SetCamera()
{
	Game::System<CameraSystem>().SetDrawCamera(hCamera_);
}

void mtgb::ImGuizmoManipulator::Initialize()
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

void mtgb::ImGuizmoManipulator::ShowImGui()
{
	//ImGuizmo�̑��샂�[�h���w��


	if (InputUtil::GetMouseDown(MouseCode::Left))
	{
		// ����Alt�L�[�������Ă���ꍇ�̓^�[�Q�b�g�̕ύX�����Ȃ�
		if (InputUtil::GetKey(KeyCode::LeftMenu) == false)
		{
			if ((!ImGuizmo::IsViewManipulateHovered() && !ImGuizmo::IsOver()) && IsMouseInWindow(MTImGui::Instance().GetName(ShowType::SceneView)) || !pTargetTransform_)
			{
				SelectTransform();
			}
		}
	}

	DrawTransformGuizmo();

	if (ImGui::Checkbox("AdjustTargetDirection_", &adjustTargetDirection_))
	{

	}
	if (ImGui::RadioButton("Translate", operation_ == ImGuizmo::TRANSLATE))
	{
		operation_ = ImGuizmo::TRANSLATE;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", operation_ == ImGuizmo::ROTATE))
	{
		operation_ = ImGuizmo::ROTATE;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", operation_ == ImGuizmo::SCALE))
	{
		operation_ = ImGuizmo::SCALE;
	}

	if (ImGui::RadioButton("Local", mode_ == ImGuizmo::LOCAL))
	{
		mode_ = ImGuizmo::LOCAL;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("World", mode_ == ImGuizmo::WORLD))
	{
		mode_ = ImGuizmo::WORLD;
	}

	ImVec2 mousePos = ImGui::GetMousePos();
	ImVec2 windowPos = ImGui::GetWindowPos();
	ImVec2 localPos = ImVec2(mousePos.x - windowPos.x, mousePos.y - windowPos.y);

	MTImGui::Instance().TypedShow(&pCameraTransform_->position, "cameraPos");
	MTImGui::Instance().TypedShow(&pCameraTransform_->rotate, "cameraRot");
	MTImGui::Instance().DirectShow([this]()
		{
			ImGui::InputFloat4("quat", pCameraTransform_->rotate.f);
		}, "CameraQuat", ShowType::Inspector);
}

void mtgb::ImGuizmoManipulator::FollowTarget()
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
			SpinCamera(followDistance_);
		}
	}

}

void mtgb::ImGuizmoManipulator::UpdateCamera(const char* _name)
{
	constexpr float ANGLE_SPEED{ DirectX::XMConvertToRadians(100.f) };
	FollowTarget();
	if (!ImGui::IsWindowFocused())
	{
		return;
	}
	if (IsMouseInWindow(_name))
	{
		if (InputUtil::GetMouse(MouseCode::Right) || InputUtil::GetMouse(MouseCode::Middle) || InputUtil::GetMouse(MouseCode::Left))
		{
			

			if (InputUtil::GetMouseDown(MouseCode::Middle))
			{
				cameraOperation_ = CameraOperation::Translate;
			}
			else if (InputUtil::GetMouseDown(MouseCode::Right))
			{
				cameraOperation_ = CameraOperation::Rotate;
				InitializeSpinAnglesFromCurrentPosition();

			}
			else if (InputUtil::GetMouseDown(MouseCode::Left) && InputUtil::GetKey(KeyCode::LeftMenu))
			{
				cameraOperation_ = CameraOperation::Spin;
				// Spin����p�̊p�x������ - ���݂̃J�����ʒu����t�Z
				InitializeSpinAnglesFromCurrentPosition();
			}
			
		}
		else
		{
			cameraOperation_ = CameraOperation::None;
		}
	}
	

	
	if (cameraOperation_ == CameraOperation::Rotate)
	{
		Vector3 mouseMove = InputUtil::GetMouseMove();
		if (mouseMove.Size() != 0)
		{
			// �}�E�X�ړ��ʂ��p�x�ɕϊ�
			spinAngleY_ += mouseMove.x * rotateSensitivity_ * Time::DeltaTimeF(); // �����p�x
			spinAngleX_ += mouseMove.y * rotateSensitivity_ * Time::DeltaTimeF(); // �����p�x
			
			// �����p�x�𐧌�
			/*const float MAX_VERTICAL = DirectX::XMConvertToRadians(89.0f);
			spinAngleX_ = std::clamp(spinAngleX_, -MAX_VERTICAL, MAX_VERTICAL);*/

			static const float ROTATE_DISTANCE = 0.01f;
			// �{����0.0f��n���Ă��̏�ŉ�]����������������肭�����Ȃ�����
			SpinCamera(ROTATE_DISTANCE);
		}
	}

	if (cameraOperation_ == CameraOperation::Translate)
	{
		Vector3 mouseMove = InputUtil::GetMouseMove();
		if (mouseMove.Size()  != 0)
		{
			// �J�����̉E�A��x�N�g��
			Vector3 right = pCameraTransform_->Right();
			Vector3 up = pCameraTransform_->Up();

			// �ړ��ʂ�����
			Vector3 move = right * -mouseMove.x + up * mouseMove.y;

			pCameraTransform_->position += move * moveSpeed_ * Time::DeltaTimeF();
		}
	}

	if (cameraOperation_ == CameraOperation::None)
	{
		Vector3 mouseMove = InputUtil::GetMouseMove();
		if (mouseMove.Size() != 0)
		{
			Vector3 forward = pCameraTransform_->Forward();

			Vector3 move = forward * mouseMove.z;

			pCameraTransform_->position += move * moveSpeed_ * Time::DeltaTimeF();
		}
	}

	if (cameraOperation_ == CameraOperation::Spin)
	{
		Vector3 mouseMove = InputUtil::GetMouseMove();
		spinAngleX_ += mouseMove.y * spinSpeed_ * Time::DeltaTimeF();
		spinAngleY_ += mouseMove.x * spinSpeed_ * Time::DeltaTimeF();

		SpinCamera(spinDistance_);
	}

}