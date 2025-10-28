#include "ImGuiEditor.h"
#include "ReleaseUtility.h"
#include "RegisterComponents.h"
#include "InputData.h"
#include "GameObjectGenerator.h"
#include "AddComponentCommand.h"
#include "TypeRegistry.h"
mtgb::ImGuiEditor::ImGuiEditor()
	: ImGuiShowable("ImGuiEditor",ShowType::Editor)
{
	pCommandHistory_ = new CommandHistoryManagerWrapper(new CommandHistoryManager());
	pComponentFactory_ = new ComponentFactory();
	// コンポーネントの作成関数を登録
	mtgb::RegisterComponents(pComponentFactory_);

	pManipulator_ = new ImGuizmoManipulator([this](Command* _command) { pCommandHistory_->ExecuteCommand(_command); },*pComponentFactory_);

	TypeRegistry::Instance().RegisterCommandListener({ [this](Command* _command) { pCommandHistory_->ExecuteCommand(_command); } });
}

mtgb::ImGuiEditor::~ImGuiEditor()
{
	SAFE_DELETE(pComponentFactory_);
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

	if (InputUtil::GetKey(KeyCode::LeftControl))
	{
		if (InputUtil::GetKeyDown(KeyCode::Z))
		{
			pCommandHistory_->UndoCommand();
		}
		if (InputUtil::GetKeyDown(KeyCode::Y))
		{
			pCommandHistory_->RedoCommand();
		}
	}
}

void mtgb::ImGuiEditor::ShowImGui()
{
	pCommandHistory_->DrawImGuiStack();
	ShowAddComponentDialog(pManipulator_->GetSelectedEntityId());
	ShowGenerateGameObjectButton();
}

void mtgb::ImGuiEditor::AddComponent(const std::type_index& _componentType, EntityId _entityId)
{
	// コンポーネント作成成功
	AddComponentCommand* cmd = new AddComponentCommand(_entityId, _componentType, nullptr, *pComponentFactory_);
	pCommandHistory_->ExecuteCommand(cmd);
}

void mtgb::ImGuiEditor::ShowAddComponentDialog(EntityId _entityId)
{
	std::vector<std::type_index> registeredTypes;
	pComponentFactory_->GetRegisteredTypes(registeredTypes);

	for (const auto& typeInfo : registeredTypes)
	{
		if (ImGui::Button(typeInfo.name()))
		{
			AddComponent(typeInfo, _entityId);
		}
	}
}

void mtgb::ImGuiEditor::ShowGenerateGameObjectButton()
{
	if (ImGui::Button("GenerateGameObject"))
	{
		GameObjectGenerator::GeneratePrimitive([this](Command* _command) { pCommandHistory_->ExecuteCommand(_command); },*pComponentFactory_,PrimitiveType::Box);
	}
}


