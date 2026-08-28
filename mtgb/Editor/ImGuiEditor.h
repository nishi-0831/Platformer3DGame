#pragma once
#include "Core/ISystem.h"
#include "ImGuizmoManipulator.h"
#include "Editor/ImGuiShowable.h"
#include <filesystem>
namespace mtgb
{
	class ImGuiEditor : public ISystem, public ImGuiShowable
	{
	  public:
		ImGuiEditor();
		~ImGuiEditor();

		void Initialize() override;
		void Release() override;
		void Update() override;
		void ShowImGui() override;
		/// <summary>
		/// メニューバーを表示
		/// </summary>
		void ShowMenuBar();
		/// <summary>
		/// インスペクターを表示
		/// </summary>
		void ShowInspector();
		/// <summary>
		/// 操作説明画面を表示
		/// </summary>
		void ShowHelpMenuWindow();
		/// <summary>
		/// 選択オブジェクトのアウトラインを描画
		/// </summary>
		void DrawSelectedObjectOutline();
		/// <summary>
		/// 選択中のエンティティのIDを返す
		/// </summary>
		/// <returns></returns>
		std::span<EntityId> GetSelectedEntityId();
		ImGuizmo::OPERATION GetOperation();

	  private:
		/// <summary>
		/// ステージデータを保存する
		/// 編集中のファイルがあるなら上書き、ないなら新規作成
		/// </summary>
		void SaveMapData();
		/// <summary>
		/// ステージデータを新規作成し、それを編集状態にする
		/// </summary>
		void SaveMapDataAs();
		/// <summary>
		/// ステージデータをコピーする
		/// </summary>
		void SaveCopyMapDataAs();
		/// <summary>
		/// ステージデータを読み込む
		/// </summary>
		void LoadMapData();
		/// <summary>
		/// 現在編集中のステージを再生する
		/// </summary>
		void PlayScene();
		/// <summary>
		/// 現在再生中のステージを停止する
		/// </summary>
		void StopScene();
		/// <summary>
		/// 現在選択されているゲームオブジェクトの複製を行う
		/// </summary>
		void DuplicateGameObject();
		/// <summary>
		/// エンティティにコンポーネントを追加する
		/// </summary>
		/// <param name="_componentType">追加するコンポーネントのtype_index</param>
		/// <param name="_entityId">追加するエンティティのID</param>
		void AddComponent(std::type_index _componentType, EntityId _entityId);
		/// <summary>
		/// コンポーネント追加のダイアログを表示
		/// </summary>
		/// <param name="_entityId">追加するエンティティのID</param>
		void ShowAddComponentDialog(EntityId _entityId);
		void ShowGenerateGameObjectButton();
		/// <summary>
		/// 指定されたゲームオブジェクトを選択
		/// </summary>
		/// <param name="_entityId"></param>
		void SelectGameObject(EntityId _entityId);
		// マニピュレーター
		ImGuizmoManipulator* pManipulator_;
		// 編集中のステージファイルのパス
		std::filesystem::path editingStagePath_;
		// 編集中のステージデータ
		nlohmann::json tmpStageData_;

		// グリッドの中心からの距離(グリッドの全体サイズは gridHalfExtent_ * 2)
		float gridHalfExtent_;
		// グリッドの分割数
		int gridDivisionNum_;
		// エディタに表示中のオブジェクト名
		std::string inspectedObjectName_;
	};
} // namespace mtgb
