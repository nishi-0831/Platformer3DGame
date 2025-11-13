#include "ImGuiEditor.h"
#include "ReleaseUtility.h"
#include "RegisterComponents.h"
#include "RegisterCommonGameObjectType.h"
#include "../Source/RegisterGameObjectType.h"
#include "InputData.h"
#include "GameObjectGenerator.h"
#include "AddComponentCommand.h"
#include "TypeRegistry.h"
#include "WindowContextUtil.h"
#include <fstream>
#include "SceneSystem.h"
#include "GameScene.h"
#include "Debug.h"
mtgb::ImGuiEditor::ImGuiEditor()
	: ImGuiShowable("ImGuiEditor",ShowType::Editor)
{
	pCommandHistory_ = new CommandHistoryManagerWrapper(new CommandHistoryManager());

	commandListener_ = [this](Command* _command)
		{
			pCommandHistory_->ExecuteCommand(_command); 
			id = _command->GetCommandTargetEntityId();
		};
	pManipulator_ = new ImGuizmoManipulator(commandListener_);

	GameObjectGenerator::RegisterCommandListener(commandListener_);
	TypeRegistry::Instance().RegisterCommandListener(commandListener_);
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
		if (InputUtil::GetKeyDown(KeyCode::S))
		{
			SaveMapData();
		}
		if (InputUtil::GetKeyDown(KeyCode::O))
		{
			LoadMapData();
		}
		if (InputUtil::GetKeyDown(KeyCode::D))
		{
			DuplicateGameObject();
		}
			
	}
}

void mtgb::ImGuiEditor::ShowImGui()
{
	pCommandHistory_->DrawImGuiStack();
	//ShowAddComponentDialog(pManipulator_->GetSelectedEntityId());
	ShowGenerateGameObjectButton();
}

void mtgb::ImGuiEditor::SaveMapData()
{
	TCHAR fileName[255] = "";
	OPENFILENAME ofn = {0};

	ofn.lStructSize = sizeof(ofn);

	ofn.hwndOwner = WinCtxRes::GetHWND(WindowContext::First);
	ofn.lpstrFilter = "JSONファイル(*.json)\0*.json";
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = 255;
	ofn.Flags = OFN_OVERWRITEPROMPT;

	if (GetSaveFileName(&ofn))
	{
		std::ofstream openFile(fileName);

		nlohmann::json j = Game::System<SceneSystem>().GetActiveScene()->SerializeGameObjects();
		int width = 4;
		openFile << std::setw(width) << j;

		openFile.close();

		LOGIMGUI_CAT("Editor", "File Saved");
	}
}

void mtgb::ImGuiEditor::LoadMapData()
{
	TCHAR fileName[255] = "";
	OPENFILENAME ifn = { 0 };

	ifn.lStructSize = sizeof(ifn);
	ifn.hwndOwner = WinCtxRes::GetHWND(WindowContext::First);
	ifn.lpstrFilter = "JSONファイル(*.json)\0*.json";
	ifn.lpstrFile = fileName;
	ifn.nMaxFile = 255;

	if (GetOpenFileName(&ifn) == false)
		return;

	std::ifstream inputFile(fileName);
	if (inputFile.fail())
	{
		assert(false);
	}
	nlohmann::json json;
	try
	{
		inputFile >> json;
	}
	catch (const nlohmann::json::parse_error& e)
	{
		assert(false,e.what());
	}
	GameObjectGenerator::GenerateFromJson(json);
}

void mtgb::ImGuiEditor::DuplicateGameObject()
{
	EntityId currSelectedEntity = pManipulator_->GetSelectedEntityId();
	if (currSelectedEntity == INVALID_ENTITY)
		return;

	GameObject* src = Game::System<SceneSystem>().GetActiveScene()->GetGameObject(currSelectedEntity);
	std::string classTypeName = src->GetClassTypeName();
	GameObjectGenerator::Generate(classTypeName);

	std::optional<std::vector<std::type_index>> componentPoolTypes = Game::System<ComponentRegistry>().GetComponentPoolTypes(currSelectedEntity);
	if (componentPoolTypes.has_value() == false)
		return;
	
	for (std::type_index componentPoolType : componentPoolTypes.value())
	{
		IComponentPool* pComponentPool = Game::GetCP(componentPoolType);
		if (pComponentPool == nullptr)
			continue;

		pComponentPool->Copy(id, src->GetEntityId());
	}
}

void mtgb::ImGuiEditor::AddComponent(const std::type_index& _componentType, EntityId _entityId)
{
	// コンポーネント作成成功
	AddComponentCommand* cmd = new AddComponentCommand(_entityId, _componentType, nullptr, Game::System<ComponentFactory>());
	pCommandHistory_->ExecuteCommand(cmd);
}

void mtgb::ImGuiEditor::ShowAddComponentDialog(EntityId _entityId)
{
	std::vector<std::type_index> registeredTypes;
	Game::System<ComponentFactory>().GetRegisteredTypes(registeredTypes);

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
	std::vector<std::string> names = Game::System<GameObjectTypeRegistry>().GetRegisteredNames();
	for (const std::string& name : names)
	{
		if (ImGui::Button(name.c_str()))
		{
			GameObjectGenerator::Generate(name);
		}
	}
}


