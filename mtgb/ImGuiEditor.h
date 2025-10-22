#pragma once
#include "ISystem.h"
#include "ImGuizmoManipulator.h"
#include "ImGuiEditorCamera.h"
#include "CommandHistoryManagerWrapper.h"
#include "ImGuiShowable.h"
#include "ComponentFactory.h"
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
		void AddComponent(const std::type_index& _componentType, EntityId _entityId);
		void ShowAddComponentDialog(EntityId _entityId);
		ComponentFactory* pComponentFactory_;
		CommandHistoryManagerWrapper* pCommandHistory_;
		ImGuizmoManipulator* pManipulator_;
	};
}
