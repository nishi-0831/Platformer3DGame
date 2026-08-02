#pragma once
#include "ImGuiShowable.h"
#include "Matrix4x4.h"

#include "ImGui/imgui_impl_win32.h"
#include "ImGui/ImGuizmo.h"
#include <functional>
#include "Command.h"
#include "GameObjectSelectionEvent.h"
#include "SelectionCommand.h"
#include "Transform.h"
#include "ComponentFactory.h"
namespace mtgb
{
	class GameObject;
	struct Vector3;
	class ImGuizmoManipulator : public ImGuiShowable
	{
	  public:
		ImGuizmoManipulator();
		~ImGuizmoManipulator();

		void Initialize();
		void Update();
		void ShowImGui() override;

		void Select(EntityId _id);
		void Deselect();
		EntityId GetSelectedEntityId();

	  private:
		void UpdateManpulator();
		void UpdateOperationMode();
		void GenerateCommand(const GameObjectSelectedEvent& _event);
		void GenerateCommand(const GameObjectDeselectedEvent& _event);
		void SubscribeEvents();

		void DrawTransformGuizmo();
		void DrawViewCube();
		void Calculate();

		Transform* pTargetTransform_;
		ImGuizmo::OPERATION operation_;
		ImGuizmo::MODE mode_;

		float worldMat_[16], viewMat_[16], projMat_[16];
		Matrix4x4 worldMatrix4x4, viewMatrix4x4_, projMatrix4x4_;
		DirectX::XMFLOAT4X4 float4x4_;
		// ビューギズモ(カメラの視点を操作するのに使う立方体)のサイズ
		ImVec2 viewGuizmoSize_;
		// カメラの方向を特定の軸にスナップさせるときに使用する基準点
		// カメラの正面方向からこの値分伸ばした点を基準として回転する
		float snapDistanceFromCamera_;
		bool wasUsing_;
		bool isUsing_;
		TransformMemento* pTargetPrevTransformMemento_;
	};
} // namespace mtgb
