#pragma once
#include "ISystem.h"
#include "ImGuizmoManipulator.h"
#include "ImGuiShowable.h"
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

		void ShowMenuBar();

	  private:
		void SaveMapData();
		void SaveMapDataAs();
		void SaveCopyMapDataAs();
		void LoadMapData();
		void PlayScene();
		void StopScene();
		/// <summary>
		/// 現在選択されているゲームオブジェクトの複製を行う
		/// </summary>
		void DuplicateGameObject();
		void AddComponent(std::type_index _componentType, EntityId _entityId);
		void ShowAddComponentDialog(EntityId _entityId);
		void ShowGenerateGameObjectButton();
		ImGuizmoManipulator* pManipulator_;
		std::filesystem::path editingStagePath_;
		nlohmann::json tmpStageData_;
	};
} // namespace mtgb
