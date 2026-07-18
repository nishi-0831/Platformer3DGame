#include "ImGuiEditor.h"
#include "ReleaseUtility.h"
#include "RegisterCommonGameObjectType.h"
#include "../Source/RegisterGameObjectType.h"
#include "InputData.h"
#include "GameObjectGenerator.h"
#include "AddComponentCommand.h"
#include "PropertyDisplayRegistry.h"
#include "WindowContextUtil.h"
#include <fstream>
#include "SceneSystem.h"
#include "GameScene.h"
#include "Debug.h"
#include "GameTime.h"
#include "CommandHistoryManager.h"
#include "MTImGui.h"
#include "../Source/Scenes/SampleScene.h"
#include "../Source/StageEditScene.h"

nlohmann::json GetStageJson(std::filesystem::path filePath)
{
	std::ifstream inputFile(filePath);
	if (inputFile.fail())
	{
		assert(false);
		return nlohmann::json();
	}
	nlohmann::json json;
	try
	{
		inputFile >> json;
		return json;
	}
	catch (const nlohmann::json::parse_error& e)
	{
		const char* errMsg = e.what();
		assert(false && errMsg);
	}
	return nlohmann::json();
}

mtgb::ImGuiEditor::ImGuiEditor()
	: ImGuiShowable("ImGuiEditor", ShowType::EDITOR, INVALID_ENTITY, ImGuiShowable::Scope::GLOBAL)
	, editingStagePath_ {}
	, tmpStageData_ {}
{

	pManipulator_ = new ImGuizmoManipulator();

	PropertyDisplayRegistry::Instance().RegisterCommandListener(
		[](Command* _command)
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
			Time::StabilizeDeltaTime();
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
	ShowAddComponentDialog(pManipulator_->GetSelectedEntityId());
	ShowGenerateGameObjectButton();
}

void mtgb::ImGuiEditor::SaveMapData()
{
	TCHAR fileName[255] = "";
	OPENFILENAME ofn	= { 0 };

	ofn.lStructSize = sizeof(ofn);

	ofn.hwndOwner	= WinCtxRes::GetHWND(WindowContext::FIRST);
	ofn.lpstrFilter = "JSON File(*.json)\0*.json";
	ofn.lpstrFile	= fileName;
	ofn.nMaxFile	= 255;
	ofn.Flags		= OFN_OVERWRITEPROMPT;

	if (GetSaveFileName(&ofn))
	{
		std::ofstream openFile(fileName);

		int width = 4;

		// 現在のステージを記録
		nlohmann::json json = Game::System<SceneSystem>().GetActiveScene()->SerializeGameObjects();
		openFile << std::setw(width) << json;

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
	ifn.lpstrFilter = "JSON File(*.json)\0*.json";
	ifn.lpstrFile	= fileName;
	ifn.nMaxFile	= 255;

	if (GetOpenFileName(&ifn) == false)
		return;

	std::filesystem::path filePath(fileName);
	if (filePath.empty())
	{
		return;
	}

	nlohmann::json json(GetStageJson(filePath));
	if (json.empty() == false)
	{
		// 編集中のファイルを記録
		editingStagePath_ = filePath;
		Game::System<SceneSystem>().Move<StageEditScene>(json);
		// 編集モードを有効にする
		Game::SetEditMode(true);
		// 現在のステージを記録
		tmpStageData_ = Game::System<SceneSystem>().GetActiveScene()->SerializeGameObjects();
	}
	Time::StabilizeDeltaTime();
}

void mtgb::ImGuiEditor::PlayScene()
{
	nlohmann::json json(Game::System<SceneSystem>().GetActiveScene()->SerializeGameObjects());

	if (json.empty() == false)
	{
		Game::System<SceneSystem>().Move<SampleScene>(json);
		tmpStageData_ = json;
		Game::SetEditMode(false);
	}
	Time::StabilizeDeltaTime();
}

void mtgb::ImGuiEditor::StopScene()
{
	// 編集モードを有効にする
	Game::SetEditMode(true);
	Game::System<SceneSystem>().Move<StageEditScene>(tmpStageData_);
	Time::StabilizeDeltaTime();
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

void mtgb::ImGuiEditor::ShowMenuBar()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save"))
			{
				SaveMapData();
			}
			if (ImGui::MenuItem("Load"))
			{
				LoadMapData();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("GameObject"))
		{
			std::vector<std::string> names = Game::System<GameObjectTypeRegistry>().GetRegisteredNames();
			for (const std::string& name : names)
			{
				if (ImGui::MenuItem(name.c_str()))
				{
					GameObjectGenerator::Generate(name);
				}
			}
			ImGui::EndMenu();
		}
		bool isEditMode = Game::IsEditMode();

		// プレイモードなら無効化
		ImGui::BeginDisabled(isEditMode == false);
		if (ImGui::Button("Play"))
		{
			PlayScene();
		}
		ImGui::EndDisabled();

		// 編集モードなら無効化
		ImGui::BeginDisabled(isEditMode);
		if (ImGui::Button("Stop"))
		{
			StopScene();
		}
		ImGui::EndDisabled();

		ImGui::EndMenuBar();
	}
}
