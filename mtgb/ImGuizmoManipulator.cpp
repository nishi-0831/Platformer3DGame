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

//std::optional<ImVec2> mtgb::ImGuizmoManipulator::WorldToImGui(const Vector3& _vec)
//{
//	Game::System<CameraSystem>().GetViewMatrix(&viewMatrix4x4_);
//	Game::System<CameraSystem>().GetProjMatrix(&projMatrix4x4_);
//
//	
//	ImVec2 windowPos = ImGui::GetWindowPos();
//	float tabBarHeight = ImGui::GetCurrentWindow()->TitleBarHeight;
//
//
//	Vector3 screenVec = DirectX::XMVector3Project(
//		_vec,
//		viewport_.TopLeftX,
//		viewport_.TopLeftY,
//		viewport_.Width,
//		viewport_.Height,
//		viewport_.MinDepth,
//		viewport_.MaxDepth,
//		projMatrix4x4_,
//		viewMatrix4x4_,
//		DirectX::XMMatrixIdentity());
//
//	if (screenVec.z < 0.0f || screenVec.z > 1.0f)
//		return std::nullopt;
//	//return ImVec2(screenVec.x, screenVec.y );
//	return ImVec2(screenVec.x + windowPos.x, screenVec.y + windowPos.y + tabBarHeight);
//}

void mtgb::ImGuizmoManipulator::SetTarget(Transform* _pTarget)
{
	pTargetTransform_ = _pTarget;
}

void mtgb::ImGuizmoManipulator::SetViewport(const D3D11_VIEWPORT& _viewport)
{
	viewport_ = _viewport;
}

void mtgb::ImGuizmoManipulator::SubscribeGameObjectSelectionEvent()
{
	EventManager& eventManager{ Game::System<EventManager>() };
	eventManager.GetEvent<GameObjectSelectedEvent>().Subscribe(
		[this](const GameObjectSelectedEvent& _event)
		{
			Game::System<TransformCP>().TryGet(pTargetTransform_, _event.entityId);
		});

	eventManager.GetEvent<SelectionClearedEvent>().Subscribe(
		[this](const SelectionClearedEvent& _event)
		{
			pTargetTransform_ = nullptr;
		});

	// 今後、同時に複数のオブジェクトを選択可能な場合になった際には修正
	eventManager.GetEvent<GameObjectDeselectedEvent>().Subscribe(
		[this](const GameObjectDeselectedEvent& _event)
		{
			pTargetTransform_ = nullptr;
		});
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

mtgb::ImGuizmoManipulator::ImGuizmoManipulator()
	:ImGuiShowable("Manipulater", ShowType::SceneView)
	, operation_{ ImGuizmo::TRANSLATE }
	, mode_{ ImGuizmo::LOCAL }
{

}

mtgb::ImGuizmoManipulator::~ImGuizmoManipulator()
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

	
}



