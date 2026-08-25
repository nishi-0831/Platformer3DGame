#include "ImGuizmoManipulator.h"
#include "Math/Vector3.h"
#include "Core/Game.h"

#include "ImGui/imgui_internal.h"
#include "ImGui/imgui.h"

#include "Camera/CameraSystem.h"
#include "Input/InputData.h"
#include "EventManager.h"
#include "Core/Entity.h"
#include "Command/GuizmoManipulatedEvent.h"
#include "MTImGui.h"
#include "Core/SceneSystem.h"
#include "Graphics/DirectX11Draw.h"
#include "Graphics/Model/Fbx.h"
#include "Components/MeshRenderer/MeshRenderer.h"
void mtgb::ImGuizmoManipulator::DrawTransformGizmo()
{
	using namespace DirectX;
	if (selectedIds_.empty())
	{
		ImGuizmo::Enable(false);
		return;
	}
	ImGuizmo::Enable(true);

	ImVec2 pos = ImGui::GetWindowPos();
	// ギズモ表示
	float tabBarHeight = ImGui::GetCurrentWindow()->TitleBarHeight;
	ImGuizmo::SetRect(pos.x, pos.y + tabBarHeight, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
	Matrix4x4 prevMat = worldMatrix4x4;

	if (ImGuizmo::Manipulate(viewMat_, projMat_, operation_, mode_, worldMat_))
	{
		DirectX::XMMATRIX mat = XMLoadFloat4x4(reinterpret_cast<XMFLOAT4X4*>(worldMat_));
		if (operation_ == ImGuizmo::OPERATION::SCALE)
		{
			XMVECTOR scale, trans, rot;
			XMMatrixDecompose(&scale, &rot, &trans, mat);
			for (int i = 0; i < selectedIds_.size(); i++)
			{
				Transform& transform = Game::System<TransformCP>().Get(selectedIds_[i]);
				transform.scale		 = XMVectorMultiply(preManipulationScales_[i], scale);
			}
		}
		else
		{
			// Δ
			Matrix4x4 delta = XMMatrixMultiply(mat, XMMatrixInverse(nullptr, prevMat));
			// P^-1
			XMMATRIX invPrev = XMMatrixInverse(nullptr, prevMat);
			// マニピュレーターの座標系に変換
			// 変化量を掛ける
			// オブジェクトの座標系に戻す
			// P^-1 ・ Δ ・ P
			XMMATRIX pivotDelta = XMMatrixMultiply(XMMatrixMultiply(invPrev, delta), prevMat);
			for (int i = 0; i < selectedIds_.size(); i++)
			{
				Transform& transform = Game::System<TransformCP>().Get(selectedIds_[i]);
				// Wi ・ (P^-1 ・ Δ ・ P)
				XMMATRIX newWorld = XMMatrixMultiply(originalWorldMatrices_[i], pivotDelta);

				// 親がいる場合の処理
				if (Transform* parent = transform.GetParent(); parent != nullptr)
				{
					Matrix4x4 parentWorld;
					parent->GenerateWorldMatrix(&parentWorld);
					XMMATRIX invParent = XMMatrixInverse(nullptr, parentWorld);
					newWorld		   = XMMatrixMultiply(invParent, newWorld);
				}

				DirectX::XMVECTOR scale, trans, rot;
				bool result = DirectX::XMMatrixDecompose(&scale, &transform.rotate.v, &trans, newWorld);
				massert(result && "XMMatrixDecomposeに失敗 @MTImGui::DrawTransformGuizmo");
				DirectX::XMStoreFloat3(&transform.position, trans);
				DirectX::XMStoreFloat3(&transform.scale, scale);
			}
		}
	}
}

void mtgb::ImGuizmoManipulator::DrawViewGizmo()
{
	using namespace DirectX;
	if (selectedIds_.empty())
	{
		ImGuizmo::Enable(false);
		return;
	}
	ImGuizmo::Enable(true);
	ImVec2 pos = ImGui::GetWindowPos();
	// ギズモ表示
	float tabBarHeight = ImGui::GetCurrentWindow()->TitleBarHeight;
	ImGuizmo::SetRect(pos.x, pos.y + tabBarHeight, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);

	ImVec2 displaySize(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
	ImVec2 viewGizmoPos(pos.x + displaySize.x - viewGizmoSize_.x, pos.y + viewGizmoSize_.y);

	float ident[16];

	ImGuizmo::ViewManipulate(
		viewMat_,
		projMat_,
		operation_,
		mode_,
		ident,
		snapDistanceFromCamera_,
		viewGizmoPos,
		viewGizmoSize_,
		IM_COL32(40, 40, 40, 255)
	);
	if (ImGuizmo::IsUsingViewManipulate())
	{
		viewMatrix4x4_		 = XMLoadFloat4x4(reinterpret_cast<XMFLOAT4X4*>(viewMat_));
		XMMATRIX worldMatrix = XMMatrixInverse(nullptr, viewMatrix4x4_);

		XMVECTOR outScale;
		XMVECTOR outRot;
		XMVECTOR outPosition;

		// 行列を分解
		XMMatrixDecompose(&outScale, &outRot, &outPosition, worldMatrix);
		Vector3 rotVec = DirectX::XMVector3Rotate(Vector3::Forward(), outRot);

		Transform& cameraTransform = Game::System<TransformCP>().Get(
			Game::System<SceneSystem>().GetActiveScene()->GetGameObject("EditorCamera")->GetEntityId()
		);
		// 上方向を+Yに指定する
		cameraTransform.rotate	 = Quaternion::LookRotation(rotVec, Vector3::Up());
		cameraTransform.position = { XMVectorGetX(outPosition), XMVectorGetY(outPosition), XMVectorGetZ(outPosition) };
	}
}

void mtgb::ImGuizmoManipulator::DrawSelectedObject(ShaderType _shaderType)
{
	for (EntityId id : selectedIds_)
	{
		MeshRenderer* pMeshRenderer = nullptr;
		Game::System<MeshRendererCP>().TryGet(pMeshRenderer, id);
		if (pMeshRenderer == nullptr)
		{
			continue;
		}
		Transform* pTransform = nullptr;
		Game::System<TransformCP>().TryGet(pTransform, id);

		if (pTransform == nullptr)
		{
			continue;
		}
		Game::System<Fbx>().Draw(pMeshRenderer->GetMesh(), *pTransform, 0, _shaderType);
	}
}

void mtgb::ImGuizmoManipulator::SubscribeEvents()
{
	EventManager& eventManager { Game::System<EventManager>() };
	// ゲームオブジェクト選択イベント
	eventManager.GetEvent<GameObjectSelectedEvent>().Subscribe(
		[this](const GameObjectSelectedEvent& _event)
		{
			GenerateSelectedCommand(_event);
		},
		EventScope::GLOBAL
	);

	// 選択解除イベント
	eventManager.GetEvent<GameObjectDeselectedEvent>().Subscribe(
		[this](const GameObjectDeselectedEvent& _event)
		{
			GenerateDeselectedCommand(_event);
		},
		EventScope::GLOBAL
	);

	// 選択全解除イベント
	eventManager.GetEvent<SelectionClearedEvent>().Subscribe(
		[this](const SelectionClearedEvent& _event)
		{
			GenerateDeselectedCommand({ .entityIds = selectedIds_ });
		},
		EventScope::GLOBAL
	);

	// ゲームオブジェクト作成イベント
	eventManager.GetEvent<GameObjectCreatedEvent>().Subscribe(
		[this](const GameObjectCreatedEvent& _event)
		{
			Select(std::vector<EntityId> { _event.entityId }, SelectionMode::REPLACE);
		},
		EventScope::GLOBAL
	);

	// ゲームオブジェクト削除イベント
	eventManager.GetEvent<GameObjectDeletedEvent>().Subscribe(
		[this](const GameObjectDeletedEvent& _event)
		{
			// 削除されたゲームオブジェクトを選択解除
			Deselect(std::vector<EntityId> { _event.entityId });
		},
		EventScope::GLOBAL
	);
}

void mtgb::ImGuizmoManipulator::CalculateGizmoMatrix()
{
	using namespace DirectX;
	Vector3 center { 0, 0, 0 };
	originalWorldMatrices_.clear();

	// マニピュレーターの座標を計算する。選択しているオブジェクトの座標を足して、その数で割る
	int count = 0;
	for (EntityId id : selectedIds_)
	{
		Transform& trans = Game::System<TransformCP>().Get(id);
		Matrix4x4 worldMat;
		trans.GenerateWorldMatrix(&worldMat);
		originalWorldMatrices_.push_back(worldMat);

		center.x += XMVectorGetX(worldMat.r[3]);
		center.y += XMVectorGetY(worldMat.r[3]);
		center.z += XMVectorGetZ(worldMat.r[3]);

		count++;
	}
	if (count != 0)
	{
		center /= static_cast<float>(count);
	}
	Game::System<mtgb::CameraSystem>().GetViewMatrix(&viewMatrix4x4_);
	Game::System<mtgb::CameraSystem>().GetProjMatrix(&projMatrix4x4_);

	worldMatrix4x4 = XMMatrixTranslation(center.x, center.y, center.z);
	// ワールド行列
	DirectX::XMStoreFloat4x4(&float4x4_, worldMatrix4x4);
	memcpy(worldMat_, &float4x4_, sizeof(worldMat_));

	// ビュー行列
	DirectX::XMStoreFloat4x4(&float4x4_, viewMatrix4x4_);
	memcpy(viewMat_, &float4x4_, sizeof(viewMat_));

	// プロジェクション行列
	DirectX::XMStoreFloat4x4(&float4x4_, projMatrix4x4_);
	memcpy(projMat_, &float4x4_, sizeof(projMat_));
}

void mtgb::ImGuizmoManipulator::CalculateOriginalScale()
{
	preManipulationScales_.clear();
	for (size_t i = 0; i < selectedIds_.size(); i++)
	{
		Transform& transform = Game::System<TransformCP>().Get(selectedIds_[i]);
		preManipulationScales_.push_back(transform.scale);
	}
}

mtgb::ImGuizmoManipulator::ImGuizmoManipulator()
	: ImGuiShowable("Manipulator", ShowType::SCENE_VIEW, INVALID_ENTITY, ImGuiShowable::Scope::GLOBAL)
	, operation_ { ImGuizmo::TRANSLATE }
	, mode_ { ImGuizmo::WORLD }
	, isUsing_ { false }
	, wasUsing_ { false }
	, viewGizmoSize_ { 75.0f, 75.0f }
	, snapDistanceFromCamera_ { 10.0f }
	, clipSpaceGizmoSize_ { 0.15f }
{
	SubscribeEvents();
}

mtgb::ImGuizmoManipulator::~ImGuizmoManipulator() {}

void mtgb::ImGuizmoManipulator::Initialize()
{
	MTImGui::Register(this);
}

void mtgb::ImGuizmoManipulator::Update()
{
	ImGuizmo::AllowAxisFlip(false);
	ImGuizmo::SetGizmoSizeClipSpace(clipSpaceGizmoSize_);

	UpdateOperationMode();
	UpdateManipulator();
}

void mtgb::ImGuizmoManipulator::ShowImGui()
{
	// ImGuizmoの操作モードを指定
	CalculateGizmoMatrix();

	DrawTransformGizmo();
	DrawViewGizmo();

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

void mtgb::ImGuizmoManipulator::Select(std::span<const EntityId> _entityIds, SelectionMode _mode)
{
	if (_entityIds.empty())
	{
		DeselectAll();
		return;
	}

	ImGuizmo::Enable(true);
	if (_mode == SelectionMode::ADD)
	{
		for (auto id : _entityIds)
		{
			if (selectedIndex_.contains(id) == false)
			{
				selectedIndex_[id] = selectedIds_.size();
				selectedIds_.push_back(id);
			}
		}
	}
	else if (_mode == SelectionMode::REPLACE)
	{
		selectedIds_.clear();
		selectedIndex_.clear();
		for (auto id : _entityIds)
		{
			selectedIndex_[id] = selectedIds_.size();
			selectedIds_.push_back(id);
		}
	}
}

void mtgb::ImGuizmoManipulator::Deselect(std::span<const EntityId> _entityIds)
{
	for (auto id : _entityIds)
	{
		// 選択中のEntityIdを除去する
		if (selectedIndex_.contains(id))
		{
			size_t selectedEntityIdx = selectedIndex_[id];
			EntityId backEntityId	 = selectedIds_.back();

			std::swap(selectedIds_[selectedIndex_[id]], selectedIds_.back());
			selectedIds_.pop_back();
			selectedIndex_.erase(id);
			selectedIndex_[backEntityId] = selectedEntityIdx;
		}
	}
}

void mtgb::ImGuizmoManipulator::DeselectAll()
{
	selectedIds_.clear();
}

std::span<mtgb::EntityId> mtgb::ImGuizmoManipulator::GetSelectedEntityId()
{
	return selectedIds_;
}

ImGuizmo::OPERATION mtgb::ImGuizmoManipulator::GetOperation()
{
	return operation_;
}

void mtgb::ImGuizmoManipulator::DrawSelectedObjectOutline()
{
	DirectX11Draw::SetIsWriteToRenderTarget(false);
	DirectX11Draw::SetStencilMode(StencilMode::WriteSelected);
	DrawSelectedObject(ShaderType::FBX_PARTS);

	DirectX11Draw::SetIsWriteToRenderTarget(true);
	DirectX11Draw::SetStencilMode(StencilMode::DrawOutline);
	DrawSelectedObject(ShaderType::OUTLINE);

	DirectX11Draw::SetStencilMode(StencilMode::DEFAULT);
}

void mtgb::ImGuizmoManipulator::UpdateManipulator()
{
	isUsing_ = ImGuizmo::IsUsing();
	if (isUsing_ == false)
	{
		preManipulationScales_.clear();
		for (size_t i = 0; i < selectedIds_.size(); i++)
		{
			Transform& transform = Game::System<TransformCP>().Get(selectedIds_[i]);
			preManipulationScales_.push_back(transform.scale);
		}
	}
	// ギズモを使用 (動かしていなくても長押しを使用状態とみなす)
	if (wasUsing_ == false && isUsing_ == true)
	{
		SAFE_CLEAR_CONTAINER_DELETE(transformMementos_);
		preManipulationScales_.clear();
		for (size_t i = 0; i < selectedIds_.size(); i++)
		{
			Transform& transform = Game::System<TransformCP>().Get(selectedIds_[i]);
			transformMementos_.push_back(transform.SaveToMemento());

			preManipulationScales_.push_back(transform.scale);
		}
	}

	// ギズモの使用を終了
	if (wasUsing_ == true && isUsing_ == false)
	{
		std::vector<TransformMemento*> currTransformMementos;
		for (size_t i = 0; i < selectedIds_.size(); i++)
		{
			Transform& transform = Game::System<TransformCP>().Get(selectedIds_[i]);
			currTransformMementos.push_back(transform.SaveToMemento());
		}
		GuizmoManipulateCommand* cmd = new GuizmoManipulateCommand(transformMementos_, currTransformMementos);
		Game::System<CommandHistoryManager>().ExecuteCommand(cmd);

		// メモリの解放はコマンドが行うと断定して、コンテナを空にする
		transformMementos_.clear();
	}
	wasUsing_ = isUsing_;
}

void mtgb::ImGuizmoManipulator::UpdateOperationMode()
{
	if (ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))
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
}

void mtgb::ImGuizmoManipulator::GenerateSelectedCommand(const GameObjectSelectedEvent& _event)
{
	SelectionCommand* cmd = new SelectionCommand(_event.entityIds, _event.selectionMode, *this);
	Game::System<CommandHistoryManager>().ExecuteCommand(cmd);
}

void mtgb::ImGuizmoManipulator::GenerateDeselectedCommand(const GameObjectDeselectedEvent& _event)
{
	DeselectionCommand* cmd = new DeselectionCommand(_event.entityIds, *this);
	Game::System<CommandHistoryManager>().ExecuteCommand(cmd);
}
