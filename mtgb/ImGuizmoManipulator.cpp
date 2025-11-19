#include "ImGuizmoManipulator.h"
#include "GameObject.h"
#include "Vector3.h"
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
#include "GuizmoManipulatedEvent.h"
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

void mtgb::ImGuizmoManipulator::SubscribeEvents()
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

	// 今後、同時に複数のオブジェクトを選択可能な場合になった際には修正
	eventManager.GetEvent<GameObjectDeselectedEvent>().Subscribe(
		[this](const GameObjectDeselectedEvent& _event)
		{
			GenerateCommand(_event);
		}, EventScope::Global);

	// ゲームオブジェクト作成イベント
	eventManager.GetEvent<GameObjectCreatedEvent>().Subscribe(
		[this](const GameObjectCreatedEvent& _event)
		{
			GenerateCommand(_event);
		},EventScope::Global);
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
	, isUsing_{false}
	, wasUsing_{false}
{
	SubscribeEvents();
}

mtgb::ImGuizmoManipulator::~ImGuizmoManipulator()
{	
}

void mtgb::ImGuizmoManipulator::Initialize()
{
}

void mtgb::ImGuizmoManipulator::Update()
{
	UpdateManpulator();
	UpdateOperationMode();
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

	isUsing_ = ImGuizmo::IsUsing();
	
	std::string text = isUsing_ ? "true" : "false";
	ImGui::Text("%s", text.c_str());
}



void mtgb::ImGuizmoManipulator::Select(EntityId _id)
{
	if (_id == INVALID_ENTITY)
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
	if (pTargetTransform_ == nullptr) return INVALID_ENTITY;
	return pTargetTransform_->GetEntityId();
}

void mtgb::ImGuizmoManipulator::UpdateManpulator()
{
	isUsing_ = ImGuizmo::IsUsing();

	// ギズモを使用 (動かしていなくても長押しを使用状態とみなす)
	if (wasUsing_ == false && isUsing_ == true)
	{
		if (pTargetTransform_ == nullptr)
			return;
		pTargetPrevTransformMemento_ = pTargetTransform_->SaveToMemento();
	}

	// ギズモの使用を終了
	if (wasUsing_ == true && isUsing_ == false)
	{
		if (pTargetPrevTransformMemento_ == nullptr || pTargetTransform_ == nullptr)
			return;

		TransformMemento* memento = pTargetTransform_->SaveToMemento();
		GuizmoManipulateCommand* event = new GuizmoManipulateCommand(pTargetPrevTransformMemento_, memento, Game::GetComponentFactory());
		commandListener_(event);
	}

	wasUsing_ = isUsing_;
}

void mtgb::ImGuizmoManipulator::UpdateOperationMode()
{
	if (InputUtil::GetKeyDown(KeyCode::W))
	{
		operation_ = ImGuizmo::TRANSLATE;
	}
	if (InputUtil::GetKeyDown(KeyCode::E))
	{
		operation_ = ImGuizmo::SCALE;
	}
	if (InputUtil::GetKeyDown(KeyCode::R))
	{
		operation_ = ImGuizmo::ROTATE;
	}
}

void mtgb::ImGuizmoManipulator::GenerateCommand(const GameObjectSelectedEvent& _event)
{
	commandListener_(new SelectionCommand((_event.entityId),
		[this](EntityId _entityId)
		{
			Select(_entityId);
		},
		[this](EntityId _entityId)
		{
			Deselect();
		}));
}

void mtgb::ImGuizmoManipulator::GenerateCommand(const GameObjectDeselectedEvent& _event)
{
	commandListener_(new DeselectionCommand((_event.entityId),
		[this](EntityId _entityId)
		{
			Deselect();
		},
		[this](EntityId _entityId)
		{
			Select(_entityId);
		}));
}

void mtgb::ImGuizmoManipulator::GenerateCommand(const GameObjectCreatedEvent& _event)
{
	commandListener_(new SelectionCommand((_event.entityId),
		[this](EntityId _entityId)
		{
			Select(_entityId);
		},
		[this](EntityId _entityId)
		{
			Deselect();
		}));
}


