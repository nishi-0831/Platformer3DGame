#include "ImGuiEditor.h"
#include "ReleaseUtility.h"
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
#include "GameTime.h"
#include "CommandHistoryManager.h"
#include "MeshRenderer.h"
mtgb::ImGuiEditor::ImGuiEditor()
	: ImGuiShowable("ImGuiEditor", ShowType::EDITOR)
{

	pManipulator_ = new ImGuizmoManipulator();

	PropertyDisplayRegistry::Instance().RegisterCommandListener(
		[this](Command* _command)
		{
			Game::System<CommandHistoryManager>().ExecuteCommand(_command);
		}
	);
}

mtgb::ImGuiEditor::~ImGuiEditor()
{
	SAFE_DELETE(pManipulator_);
}

void mtgb::ImGuiEditor::Initialize() {}

void mtgb::ImGuiEditor::Release() {}

void mtgb::ImGuiEditor::Update()
{
	pManipulator_->Update();

	if (InputUtil::GetKey(KeyCode::LEFT_CONTROL))
	{
		if (InputUtil::GetKeyDown(KeyCode::Z))
		{
			Game::System<CommandHistoryManager>().UndoCommand();
		}
		if (InputUtil::GetKeyDown(KeyCode::Y))
		{
			Game::System<CommandHistoryManager>().RedoCommand();
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
	if (InputUtil::GetKeyDown(KeyCode::DELETE))
	{
		// マニピュレータが選択しているゲームオブジェクトを取得
		GameObjectGenerator::Delete(pManipulator_->GetSelectedEntityId());
	}
}

void mtgb::ImGuiEditor::ShowImGui()
{
	// Game::System<CommandHistoryManager>().DrawImGuiStack();
	ShowAddComponentDialog(pManipulator_->GetSelectedEntityId());
	ShowGenerateGameObjectButton();
}

void mtgb::ImGuiEditor::SaveMapData()
{
	TCHAR fileName[255] = "";
	OPENFILENAME ofn	= { 0 };

	ofn.lStructSize = sizeof(ofn);

	ofn.hwndOwner	= WinCtxRes::GetHWND(WindowContext::FIRST);
	ofn.lpstrFilter = "JSONファイル(*.json)\0*.json";
	ofn.lpstrFile	= fileName;
	ofn.nMaxFile	= 255;
	ofn.Flags		= OFN_OVERWRITEPROMPT;

	if (GetSaveFileName(&ofn))
	{
		std::ofstream openFile(fileName);

		nlohmann::json j = Game::System<SceneSystem>().GetActiveScene()->SerializeGameObjects();
		int width		 = 4;
		openFile << std::setw(width) << j;

		openFile.close();

		LOGIMGUI_CAT("Editor", "File Saved");
	}
}

void mtgb::ImGuiEditor::LoadMapData()
{
	TCHAR fileName[255] = "";
	OPENFILENAME ifn	= { 0 };

	ifn.lStructSize = sizeof(ifn);
	ifn.hwndOwner	= WinCtxRes::GetHWND(WindowContext::FIRST);
	ifn.lpstrFilter = "JSONファイル(*.json)\0*.json";
	ifn.lpstrFile	= fileName;
	ifn.nMaxFile	= 255;

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
		const char* errMsg = e.what();
		assert(false && errMsg);
	}
	GameObjectGenerator::GenerateFromJson(json);

	// 読み込み時間で値が大きくなったデルタタイムを安定させるために2フレーム待機させる
	// TODO: マジックナンバーを修正
	Time::WaitFrame(2);
}

void mtgb::ImGuiEditor::DuplicateGameObject()
{
	EntityId currSelectedEntity = pManipulator_->GetSelectedEntityId();
	if (currSelectedEntity == INVALID_ENTITY)
		return;

	GameObjectGenerator::Duplicate(currSelectedEntity);
}

void mtgb::ImGuiEditor::AddComponent(std::type_index _componentType, EntityId _entityId)
{
	// コンポーネント作成成功
	AddComponentCommand* cmd = new AddComponentCommand(_entityId, _componentType, nullptr);
	Game::System<CommandHistoryManager>().ExecuteCommand(cmd);
}

void mtgb::ImGuiEditor::ShowAddComponentDialog(EntityId _entityId)
{
	ImGui::Text("%s", "AddComponent");
	ImGui::Separator();

	std::span<const std::type_index> registeredTypes = Game::GetComponentFactory().GetRegisteredTypes();

	for (const auto& typeInfo : registeredTypes)
	{
		if (ImGui::Button(typeInfo.name()))
		{
			AddComponent(typeInfo, _entityId);
		}
	}
	ImGui::Separator();
}

void mtgb::ImGuiEditor::ShowGenerateGameObjectButton()
{
	ImGui::Text("%s", "GenerateGameObject");
	ImGui::Separator();
	std::vector<std::string> names = Game::System<GameObjectTypeRegistry>().GetRegisteredNames();
	for (const std::string& name : names)
	{
		if (ImGui::Button(name.c_str()))
		{
			GameObjectGenerator::Generate(name);
		}
	}
	ImGui::Separator();
}
