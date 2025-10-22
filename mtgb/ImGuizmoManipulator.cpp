#include "ImGuizmoManipulator.h"
#include "GameObject.h"
#include "Vector3.h"
#include "Transform.h"
#include "Game.h"
#include "ISystem.h"

#include "../ImGui\imgui_internal.h"
#include "../ImGui/imgui_impl_win32.h"
#include "../ImGui/imgui_impl_dx11.h"
#include "CameraSystem.h"
#include "InputData.h"
#include "WindowContextUtil.h"
#include "QuatToEuler.h"
#include "ImGuiRenderer.h"
#include "MTImGui.h"
#include <algorithm>
#include "SceneSystem.h"
#include "EventManager.h"
#include "Debug.h"
#include "Entity.h"
namespace
{
	
}

void mtgb::ImGuizmoManipulator::DrawTransformGuizmo()
{
	if (!pTargetTransform_)
	{
		ImGuizmo::Enable(false);
		return;
	}
	ImGuizmo::Enable(true);

	Calculate();

	uintptr_t ptrId = reinterpret_cast<uintptr_t>(pTargetTransform_);
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

void mtgb::ImGuizmoManipulator::SubscribeGameObjectSelectionEvent()
{
	EventManager& eventManager{ Game::System<EventManager>() };
	eventManager.GetEvent<GameObjectSelectedEvent>().Subscribe(
		[this](const GameObjectSelectedEvent& _event)
		{
			GenerateCommand(_event);
		},EventScope::Global);

	eventManager.GetEvent<SelectionClearedEvent>().Subscribe(
		[this](const SelectionClearedEvent& _event)
		{
			
		}, EventScope::Global);

	// ����A�����ɕ����̃I�u�W�F�N�g��I���\�ȏꍇ�ɂȂ����ۂɂ͏C��
	eventManager.GetEvent<GameObjectDeselectedEvent>().Subscribe(
		[this](const GameObjectDeselectedEvent& _event)
		{
			GenerateCommand(_event);
		}, EventScope::Global);
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

mtgb::ImGuizmoManipulator::ImGuizmoManipulator(std::function<void(Command*)> _commandListener)
	:ImGuiShowable("Manipulater", ShowType::SceneView)
	, operation_{ ImGuizmo::TRANSLATE }
	, mode_{ ImGuizmo::LOCAL }
	, commandListener_{_commandListener}
{
	SubscribeGameObjectSelectionEvent();
}

mtgb::ImGuizmoManipulator::~ImGuizmoManipulator()
{	
}

void mtgb::ImGuizmoManipulator::Initialize()
{
}

void mtgb::ImGuizmoManipulator::Update()
{
}

void mtgb::ImGuizmoManipulator::ShowImGui()
{
	//ImGuizmo�̑��샂�[�h���w��

	DrawTransformGuizmo();

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
}



void mtgb::ImGuizmoManipulator::Select(EntityId _id)
{
	if (_id == INVALD_ENTITY)
	{
		ImGuizmo::Enable(false);
		pTargetTransform_ = nullptr;
	}
	else
	{
		ImGuizmo::Enable(true);
		Game::System<TransformCP>().TryGet(pTargetTransform_, _id);
	}
}

void mtgb::ImGuizmoManipulator::Deselect()
{
	ImGuizmo::Enable(false);
	pTargetTransform_ = nullptr;
}

mtgb::EntityId mtgb::ImGuizmoManipulator::GetSelectedEntityId()
{
	return pTargetTransform_->GetEntityId();
}

void mtgb::ImGuizmoManipulator::GenerateCommand(const GameObjectSelectedEvent& _event)
{
	commandListener_(new SelectionCommand((_event),
		[this](const GameObjectSelectedEvent _evt)
		{
			Select(_evt.entityId);
		},
		[this](const GameObjectSelectedEvent _evt)
		{
			Deselect();
		}));
}

void mtgb::ImGuizmoManipulator::GenerateCommand(const GameObjectDeselectedEvent& _event)
{
	commandListener_(new DeselectionCommand((_event),
		[this](const GameObjectDeselectedEvent& _evt)
		{
			Deselect();
		},
		[this](const GameObjectDeselectedEvent& _evt)
		{
			Select(_evt.entityId);
		}));
}
