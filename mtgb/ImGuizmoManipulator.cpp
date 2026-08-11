#include "ImGuizmoManipulator.h"
#include "Vector3.h"
#include "Game.h"

#include "ImGui/imgui_internal.h"
#include "ImGui/imgui.h"

#include "CameraSystem.h"
#include "InputData.h"
#include "EventManager.h"
#include "Entity.h"
#include "GuizmoManipulatedEvent.h"
#include "MTImGui.h"
#include "SceneSystem.h"
void mtgb::ImGuizmoManipulator::DrawTransformGizmo()
{
	using namespace DirectX;
	if (selectedIdList_.empty())
	{
		ImGuizmo::Enable(false);
		return;
	}
	ImGuizmo::Enable(true);

	ImVec2 pos = ImGui::GetWindowPos();
	// ギズモ表示
	float tabBarHeight = ImGui::GetCurrentWindow()->TitleBarHeight;
	ImGuizmo::SetRect(pos.x, pos.y + tabBarHeight, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);

	if (ImGuizmo::Manipulate(viewMat_, projMat_, operation_, mode_, worldMat_))
	{
		// 編集されたworldMatからposition,rotation,scaleに分解
		DirectX::XMMATRIX mat = DirectX::XMMATRIX(
			worldMat_[0],
			worldMat_[1],
			worldMat_[2],
			worldMat_[3],
			worldMat_[4],
			worldMat_[5],
			worldMat_[6],
			worldMat_[7],
			worldMat_[8],
			worldMat_[9],
			worldMat_[10],
			worldMat_[11],
			worldMat_[12],
			worldMat_[13],
			worldMat_[14],
			worldMat_[15]
		);

		Matrix4x4 delta = XMMatrixMultiply(mat, XMMatrixInverse(nullptr, selectionWorldBefore_));
		// Matrix4x4 delta = mat;
		for (int i = 0; i < selectedIdList_.size(); i++)
		{
			Transform& transform = Game::System<TransformCP>().Get(selectedIdList_[i]);

			XMMATRIX newWorld = XMMatrixMultiply(originalWorldMatrices_[i], delta);
			XMMATRIX localMat = newWorld;
			// 親がいる場合の処理
			if (Transform* parent = transform.GetParent(); parent != nullptr)
			{
				Matrix4x4 parentWorld;
				parent->GenerateWorldMatrix(&parentWorld);
				localMat = XMMatrixMultiply(XMMatrixInverse(nullptr, parentWorld), newWorld);
			}

			DirectX::XMVECTOR scale, trans, rot;
			bool result = DirectX::XMMatrixDecompose(&scale, &transform.rotate.v, &trans, localMat);
			massert(result && "XMMatrixDecomposeに失敗 @MTImGui::DrawTransformGuizmo");
			DirectX::XMStoreFloat3(&transform.position, trans);
			DirectX::XMStoreFloat3(&transform.scale, scale);
		}
	}
}

void mtgb::ImGuizmoManipulator::DrawViewGizmo()
{
	using namespace DirectX;
	if (selectedIdList_.empty())
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
	Transform& cameraTransform = Game::System<TransformCP>().Get(
		Game::System<SceneSystem>().GetActiveScene()->GetGameObject("EditorCamera")->GetEntityId()
	);

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
		memcpy(&float4x4_, viewMat_, sizeof(viewMat_));
		viewMatrix4x4_		 = XMLoadFloat4x4(&float4x4_);
		XMMATRIX worldMatrix = XMMatrixInverse(nullptr, viewMatrix4x4_);

		XMVECTOR outScale;
		XMVECTOR outRot;
		XMVECTOR outPosition;

		// 行列を分解
		XMMatrixDecompose(&outScale, &outRot, &outPosition, worldMatrix);
		Vector3 rotVec = DirectX::XMVector3Rotate(Vector3::Forward(), outRot);
		// 上方向を+Yに指定する
		cameraTransform.rotate	 = Quaternion::LookRotation(rotVec, Vector3::Up());
		cameraTransform.position = { XMVectorGetX(outPosition), XMVectorGetY(outPosition), XMVectorGetZ(outPosition) };
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
			GenerateDeselectedCommand({ .entityIds = selectedIdList_ });
		},
		EventScope::GLOBAL
	);

	// ゲームオブジェクト作成イベント
	eventManager.GetEvent<GameObjectCreatedEvent>().Subscribe(
		[this](const GameObjectCreatedEvent& _event)
		{
			Select(std::vector<EntityId> { _event.entityId }, false);
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
	// float[16]の配列を作成
	using namespace DirectX;
	Vector3 center { 0, 0, 0 };
	originalWorldMatrices_.clear();
	int count = 0;
	for (EntityId id : selectedIdList_)
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

	selectionWorldBefore_ = XMMatrixTranslation(center.x, center.y, center.z);
	worldMatrix4x4		  = selectionWorldBefore_;
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

	UpdateManipulator();
	UpdateOperationMode();
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

void mtgb::ImGuizmoManipulator::Select(std::span<const EntityId> _entityIds, bool _multiSelect)
{
	if (_entityIds.empty())
	{
		DeselectAll();
	}
	else
	{
		ImGuizmo::Enable(true);
		if (_multiSelect)
		{
			std::ranges::copy(_entityIds, std::back_inserter(selectedIdList_));
		}
		else
		{
			selectedIdList_.clear();
			selectedIdList_ = { _entityIds.begin(), _entityIds.end() };
		}
	}
}

void mtgb::ImGuizmoManipulator::Deselect(std::span<const EntityId> _entityIds)
{
	std::erase_if(
		selectedIdList_,
		[_entityIds](EntityId _id)
		{
			return std::ranges::find(_entityIds, _id) != _entityIds.end();
		}
	);
}

void mtgb::ImGuizmoManipulator::DeselectAll()
{
	selectedIdList_.clear();
}

std::span<mtgb::EntityId> mtgb::ImGuizmoManipulator::GetSelectedEntityId()
{
	return selectedIdList_;
}

void mtgb::ImGuizmoManipulator::UpdateManipulator()
{
	isUsing_ = ImGuizmo::IsUsing();

	// ギズモを使用 (動かしていなくても長押しを使用状態とみなす)
	if (wasUsing_ == false && isUsing_ == true)
	{
		SAFE_CLEAR_CONTAINER_DELETE(transformMementos_);
		for (size_t i = 0; i < selectedIdList_.size(); i++)
		{
			Transform& transform = Game::System<TransformCP>().Get(selectedIdList_[i]);
			transformMementos_.push_back(transform.SaveToMemento());
		}
	}

	// ギズモの使用を終了
	if (wasUsing_ == true && isUsing_ == false)
	{
		std::vector<TransformMemento*> currTransformMementos;
		for (size_t i = 0; i < selectedIdList_.size(); i++)
		{
			Transform& transform = Game::System<TransformCP>().Get(selectedIdList_[i]);
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
	SelectionCommand* cmd = new SelectionCommand(_event.entityIds, _event.multiSelect, *this);
	Game::System<CommandHistoryManager>().ExecuteCommand(cmd);
}

void mtgb::ImGuizmoManipulator::GenerateDeselectedCommand(const GameObjectDeselectedEvent& _event)
{
	DeselectionCommand* cmd = new DeselectionCommand(_event.entityIds, *this);
	Game::System<CommandHistoryManager>().ExecuteCommand(cmd);
}
