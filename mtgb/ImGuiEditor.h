#pragma once
#include "ISystem.h"
#include "ImGuizmoManipulator.h"
#include "ImGuiEditorCamera.h"
#include "CommandHistoryManagerWrapper.h"
#include "ImGuiShowable.h"
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
		CommandHistoryManagerWrapper* pCommandHistory_;
		ImGuizmoManipulator* pManipulator_;
	};
}
