#include "ImGuiEditor.h"
mtgb::ImGuiEditor::ImGuiEditor()
{
	pManipulator_ = new ImGuizmoManipulator([this](Command* _command) { commandHistory_->ExecuteCommand(_command); });
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


