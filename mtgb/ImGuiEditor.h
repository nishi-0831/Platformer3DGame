#pragma once
#include <unordered_map>
#include "ISystem.h"
#include "ImGuizmoManipulator.h"
#include "ImGuiShowable.h"
#include "IComponentMemento.h"
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
		void LoadMapData();
		void PlayScene();
		/// <summary>
		/// 現在選択されているゲームオブジェクトの複製を行う
		/// </summary>
		void DuplicateGameObject();
		void AddComponent(std::type_index _componentType, EntityId _entityId);
		void ShowAddComponentDialog(EntityId _entityId);
		void ShowGenerateGameObjectButton();
		ImGuizmoManipulator* pManipulator_;
		nlohmann::json GetStageJson();
	};
} // namespace mtgb
