#include "ImGuiEditor.h"
mtgb::ImGuiEditor::ImGuiEditor()
	: ImGuiShowable("ImGuiEditor",ShowType::Editor)
{
	pCommandHistory_ = new CommandHistoryManagerWrapper(new CommandHistoryManager());
	pManipulator_ = new ImGuizmoManipulator([this](Command* _command) { pCommandHistory_->ExecuteCommand(_command); });
}

mtgb::ImGuiEditor::~ImGuiEditor()
{
}

void mtgb::ImGuiEditor::Initialize()
{
}

void mtgb::ImGuiEditor::Release()
{
}

void mtgb::ImGuiEditor::Update()
{
	pManipulator_->Update();
}

void mtgb::ImGuiEditor::ShowImGui()
{
	pCommandHistory_->DrawImGuiStack();
}


