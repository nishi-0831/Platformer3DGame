#pragma once
#include "ISystem.h"
#include "ImGuizmoManipulator.h"
#include "ImGuiEditorCamera.h"
#include "CommandHistoryManager.h"
namespace mtgb
{
	class ImGuiEditor : public ISystem
	{
	public:
		ImGuiEditor();
		~ImGuiEditor();

		void Initialize() override;
		void Release() override;
		void Update() override;

	private:
		CommandHistoryManager* commandHistory_;
		ImGuizmoManipulator* pManipulator_;
	};
}
