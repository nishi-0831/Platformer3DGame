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
#include "GameObjectSelectionEvent.h"
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

	//ギズモ表示
	float tabBarHeight = ImGui::GetCurrentWindow()->TitleBarHeight;
	ImGuizmo::SetRect(pos.x, pos.y + tabBarHeight, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);

	if (ImGuizmo::Manipulate(viewMat_, projMat_, operation_, mode_, worldMat_))
	{
		//編集されたworldMatからposition,rotation,scaleに分解
		DirectX::XMMATRIX mat = DirectX::XMMATRIX(
			worldMat_[0], worldMat_[1], worldMat_[2], worldMat_[3],
			worldMat_[4], worldMat_[5], worldMat_[6], worldMat_[7],
			worldMat_[8], worldMat_[9], worldMat_[10], worldMat_[11],
			worldMat_[12], worldMat_[13], worldMat_[14], worldMat_[15]
		);

		DirectX::XMVECTOR scale, trans;
		bool result = DirectX::XMMatrixDecompose(&scale, &pTargetTransform_->rotate.v, &trans, mat);
		massert(result
			&& "XMMatrixDecomposeに失敗 @MTImGui::DrawTransformGuizmo");

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
			Select(_event.entityId);
		},EventScope::Global);

	eventManager.GetEvent<SelectionClearedEvent>().Subscribe(
		[this](const SelectionClearedEvent& _event)
		{
			Deselect();
		}, EventScope::Global);

	// 今後、同時に複数のオブジェクトを選択可能な場合になった際には修正
	eventManager.GetEvent<GameObjectDeselectedEvent>().Subscribe(
		[this](const GameObjectDeselectedEvent& _event)
		{
			Deselect();
		}, EventScope::Global);
}

void mtgb::ImGuizmoManipulator::Calculate()
{
	//float[16]の配列を作成
	pTargetTransform_->GenerateWorldMatrix(&worldMatrix4x4);
	Game::System<mtgb::CameraSystem>().GetViewMatrix(&viewMatrix4x4_);
	Game::System<mtgb::CameraSystem>().GetProjMatrix(&projMatrix4x4_);


	//ワールド行列
	DirectX::XMStoreFloat4x4(&float4x4_, worldMatrix4x4);
	memcpy(worldMat_, &float4x4_, sizeof(worldMat_));

	//ビュー行列
	DirectX::XMStoreFloat4x4(&float4x4_, viewMatrix4x4_);
	memcpy(viewMat_, &float4x4_, sizeof(viewMat_));

	//プロジェクション行列
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
	//ImGuizmoの操作モードを指定

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

	ImGui::ShowDemoWindow();
}

void mtgb::ImGuizmoManipulator::Select(mtgb::EntityId _id)
{
	ImGuizmo::Enable(true);
	Game::System<TransformCP>().TryGet(pTargetTransform_, _id);
	LOGIMGUI("Manipulator:Selected");
}

void mtgb::ImGuizmoManipulator::Deselect()
{
	ImGuizmo::Enable(false);
	pTargetTransform_ = nullptr;
}



