#pragma once
#include "ImGuiShowable.h"
#include "Math/Matrix4x4.h"

#include "ImGui/imgui_impl_win32.h"
#include "ImGui/ImGuizmo.h"
#include <functional>
#include "Editor/Command/Command.h"
#include "GameObjectSelectionEvent.h"
#include "Editor/Command/SelectionCommand.h"
#include "Transform.h"
#include "ShaderType.h"
#include "SelectionMode.h"
namespace mtgb
{
	/// <summary>
	/// ImGuizmoのギズモを操作するマニピュレーター
	/// </summary>
	class ImGuizmoManipulator : public ImGuiShowable
	{
	  public:
		ImGuizmoManipulator();
		~ImGuizmoManipulator();

		void Initialize();
		void Update();
		void ShowImGui() override;

		/// <summary>
		/// エンティティを選択する
		/// </summary>
		/// <param name="_entityIds">選択するエンティティのIDの配列</param>
		/// <param name="_multiSelect"></param>
		void Select(std::span<const EntityId> _entityIds, SelectionMode _mode);
		/// <summary>
		/// エンティティを選択解除
		/// </summary>
		/// <param name="_entityIds"></param>
		void Deselect(std::span<const EntityId> _entityIds);
		/// <summary>
		/// 現在選択中のエンティティを全て解除
		/// </summary>
		void DeselectAll();
		/// <summary>
		/// 選択中のEntityIdを返す
		/// </summary>
		/// <returns></returns>
		std::span<EntityId> GetSelectedEntityId();
		/// <summary>
		/// 現在の操作モード(Translation,Rotate,Scaleなど)を返す
		/// </summary>
		/// <returns></returns>
		ImGuizmo::OPERATION GetOperation();
		/// <summary>
		/// 選択中のオブジェクトのアウトラインを描画
		/// </summary>
		void DrawSelectedObjectOutline();

	  private:
		void UpdateManipulator();
		void UpdateOperationMode();
		void GenerateSelectedCommand(const GameObjectSelectedEvent& _event);
		void GenerateDeselectedCommand(const GameObjectDeselectedEvent& _event);
		void SubscribeEvents();

		void DrawTransformGizmo();
		void DrawViewGizmo();
		void DrawSelectedObject(ShaderType _shaderType);
		void CalculateGizmoMatrix();
		void CalculateOriginalScale();

		ImGuizmo::OPERATION operation_;
		ImGuizmo::MODE mode_;

		float worldMat_[16], viewMat_[16], projMat_[16];
		Matrix4x4 worldMatrix4x4, viewMatrix4x4_, projMatrix4x4_;
		DirectX::XMFLOAT4X4 float4x4_;
		// ビューギズモ(カメラの視点を操作するのに使う立方体)のサイズ
		ImVec2 viewGizmoSize_;
		// カメラの方向を特定の軸にスナップさせるときに使用する基準点
		// カメラの正面方向からこの値分伸ばした点を基準として回転する
		float snapDistanceFromCamera_;
		bool wasUsing_;
		bool isUsing_;
		std::vector<TransformMemento*> transformMementos_;
		// ギズモの大きさ
		// クリップ空間(-1.0～1.0)における値を指定する。0.2なら画面の10%になる
		float clipSpaceGizmoSize_;
		std::vector<EntityId> selectedIds_;
		std::unordered_map<EntityId, size_t> selectedIndex_;
		std::vector<Matrix4x4> originalWorldMatrices_;
		// 選択したオブジェクトの、ギズモを使用する直前のスケール
		std::vector<DirectX::XMVECTOR> preManipulationScales_;
	};
} // namespace mtgb
