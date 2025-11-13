#pragma once
#include <unordered_map>
#include "ISystem.h"
#include "ImGuizmoManipulator.h"
#include "ImGuiEditorCamera.h"
#include "CommandHistoryManagerWrapper.h"
#include "ImGuiShowable.h"
#include "ComponentFactory.h"
#include "IComponentMemento.h"
#include "GameObjectFactory.h"
namespace mtgb
{
	class ImGuiEditor : public ISystem , ImGuiShowable
	{
	public:
		ImGuiEditor();
		~ImGuiEditor();

		void Initialize() override;
		void Release() override;
		void Update() override;
		void ShowImGui() override;

	private:
		void SaveMapData();
		void LoadMapData();
		/// <summary>
		/// 現在選択されているゲームオブジェクトの複製を行う
		/// </summary>
		void DuplicateGameObject();
		void AddComponent(const std::type_index& _componentType, EntityId _entityId);
		void ShowAddComponentDialog(EntityId _entityId);
		void ShowGenerateGameObjectButton();
		CommandHistoryManagerWrapper* pCommandHistory_;
		ImGuizmoManipulator* pManipulator_;
		std::unordered_map<std::type_index, IComponentMemento*> defMementos_;
		std::function<void(Command*)> commandListener_;
		EntityId id;
	};
}
