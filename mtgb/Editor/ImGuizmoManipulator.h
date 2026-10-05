#pragma once
#include "Editor/ImGuiShowable.h"
#include "Math/Matrix4x4.h"

#include "ImGui/imgui_impl_win32.h"
#include "ImGui/ImGuizmo.h"
#include <functional>
#include "Editor/Command/Command.h"
#include "GameObjectSelectionEvent.h"
#include "Editor/Command/SelectionCommand.h"
#include "Components/Transform/Transform.h"
#include "Graphics/ShaderType.h"
#include "Editor/SelectionMode.h"
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
		/// <summary>
		/// ギズモの使用状態を更新する。
		/// 使用開始前・終了時の処理も行う
		/// </summary>
		void UpdateGizmoManipulationState();
		/// <summary>
		/// 操作モード(Translation,Rotate,Scaleなど)を更新
		/// </summary>
		void UpdateOperationMode();
		/// <summary>
		/// ゲームオブジェクトを選択するコマンドを生成
		/// </summary>
		/// <param name="_event"></param>
		void GenerateSelectedCommand(const GameObjectSelectedEvent& _event);
		/// <summary>
		/// ゲームオブジェクトを選択解除するコマンドを生成
		/// </summary>
		/// <param name="_event"></param>
		void GenerateDeselectedCommand(const GameObjectDeselectedEvent& _event);
		/// <summary>
		/// ゲームで発生するイベントに対する処理を登録する
		/// </summary>
		void SubscribeEvents();
		/// <summary>
		/// トランスフォームを操作する用のギズモを描画
		/// </summary>
		void DrawTransformGizmo();
		/// <summary>
		/// カメラの視点をXYZ軸に切り替えられるギズモを描画
		/// </summary>
		void DrawViewGizmo();
		/// <summary>
		/// 選択したオブジェクトを描画する
		/// アウトライン描画のために、ステンシルバッファに書き込む用
		/// </summary>
		/// <param name="_shaderType"></param>
		void DrawSelectedObject(ShaderType _shaderType);
		/// <summary>
		/// ギズモの描画に使う行列を計算
		/// </summary>
		void CalculateGizmoMatrix();

		// トランスフォームギズモの操作モード(Transform、Rotate、Scaleなど)
		ImGuizmo::OPERATION operation_;
		// トランスフォームギズモ操作の座標系(World、Local)
		ImGuizmo::MODE mode_;

		/// ギズモの描画に使う行列
		float worldMat_[16], viewMat_[16], projMat_[16];
		Matrix4x4 worldMatrix4x4, viewMatrix4x4_, projMatrix4x4_;
		DirectX::XMFLOAT4X4 float4x4_;

		// ビューギズモ(カメラの視点を操作するのに使う立方体)のサイズ
		ImVec2 viewGizmoSize_;
		// カメラの方向を特定の軸にスナップさせるときに使用する基準点
		// カメラの正面方向からこの値分伸ばした点を基準として回転する
		float snapDistanceFromCamera_;
		// 前回、トランスフォームギズモを使用していたか
		bool wasUsing_;
		// トランスフォームギズモを使用しているか
		bool isUsing_;
		/// <summary>
		/// ギズモで操作中のトランスフォームのメメント
		/// </summary>
		std::vector<TransformMemento*> transformMementos_;
		// ギズモの大きさ
		// クリップ空間(-1.0～1.0)における値を指定する。0.2なら画面の10%になる
		float clipSpaceGizmoSize_;
		/// <summary>
		/// 選択中のオブジェクトのID
		/// </summary>
		std::vector<EntityId> selectedIds_;
		/// <summary>
		/// 選択中のオブジェクトのID→selectedIds_へのインデックス
		/// </summary>
		std::unordered_map<EntityId, size_t> selectedIndex_;
		/// <summary>
		/// 選択オブジェクトの、操作前のワールド行列
		/// </summary>
		std::vector<Matrix4x4> originalWorldMatrices_;
		// 選択オブジェクトの、操作前のワールド行列
		std::vector<DirectX::XMVECTOR> preManipulationScales_;
	};
} // namespace mtgb
