#include "ImGuiEditor.h"
#include "Utility/ReleaseUtility.h"
#include "CommonGameObject/RegisterCommonGameObjectType.h"
#include "Input/InputData.h"
#include "Core/GameObject/GameObjectGenerator.h"
#include "Command/AddComponentCommand.h"
#include "PropertyDisplayRegistry.h"
#include "Window/WindowContext/WindowContextUtil.h"
#include <fstream>
#include "Core/SceneSystem.h"
#include "Core/GameScene.h"
#include "Debug.h"
#include "Core/Time/GameTime.h"
#include "Command/CommandHistoryManager.h"
#include "MTImGui.h"
#include "../Source/Scenes/SampleScene.h"
#include "../Source/StageEditScene.h"
#include "../Source/TitleScene.h"

static void ShowGameObjectMenu(const mtgb::SpawnObjectDesc& _desc)
{
	// カテゴリーが存在する場合
	if (_desc.category != "")
	{
		if (ImGui::BeginMenu(_desc.category.c_str()))
		{
			// アイテム表示
			if (ImGui::MenuItem(_desc.displayName.c_str()))
			{
				mtgb::GameObjectGenerator::Generate(_desc.typeName);
			}
			// ツールチップ
			if (ImGui::IsItemHovered())
			{
				if (_desc.tooltip.empty() == false)
				{
					ImGui::SetItemTooltip(_desc.tooltip.c_str());
				}
			}
			ImGui::EndMenu();
		}
	}
	// カテゴリーなし
	else
	{
		// アイテム表示
		if (ImGui::MenuItem(_desc.displayName.c_str()))
		{
			GameObjectGenerator::Generate(_desc.typeName);
		}
		// ツールチップ
		if (ImGui::IsItemHovered())
		{
			if (_desc.tooltip.empty() == false)
			{
				ImGui::SetItemTooltip(_desc.tooltip.c_str());
			}
		}
	}
}

static nlohmann::json GetStageJson(std::filesystem::path _filePath)
{
	std::ifstream inputFile(_filePath);
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
static void SaveStageJson(std::filesystem::path _filePath)
{
	std::ofstream openFile(_filePath);
	if (openFile.fail())
	{
		LOGIMGUI_CAT("Editor", "Failed File Save");
		assert(false);
		return;
	}
	openFile << std::setw(4) << Game::System<SceneSystem>().GetActiveScene()->SerializeGameObjects();

	openFile.close();

	LOGIMGUI_CAT("Editor", "File Saved");
}

static std::filesystem::path GetJsonFilePath()
{
	TCHAR fileName[255] = "";
	OPENFILENAME ofn	= { 0 };
	fs::path cp			= fs::current_path();
	cp.append("Stage");
	std::string str = cp.string();
	ofn.lStructSize = sizeof(ofn);

	ofn.hwndOwner		= WinCtxRes::GetHWND(WindowContext::FIRST);
	ofn.lpstrFilter		= "JSON File(*.json)\0*.json";
	ofn.lpstrFile		= fileName;
	ofn.nMaxFile		= 255;
	ofn.Flags			= OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
	ofn.lpstrInitialDir = str.c_str();
	if (GetSaveFileName(&ofn))
	{
		std::filesystem::path filePath(fileName);
		if (filePath.extension() != ".json")
		{
			filePath += ".json";
		}
		return filePath;
	}
	return std::filesystem::path();
}

mtgb::ImGuiEditor::ImGuiEditor()
	: ImGuiShowable("ImGuiEditor", ShowType::EDITOR, INVALID_ENTITY, ImGuiShowable::Scope::GLOBAL)
	, editingStagePath_ {}
	, tmpStageData_ {}
	, gridHalfExtent_ { 100.0f }
	, gridDivisionNum_ { 10 }
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

void mtgb::ImGuiEditor::Initialize()
{
	// ゲームオブジェクトが選択されたときに、それを表示対象とする
	Game::System<EventManager>().GetEvent<GameObjectSelectedEvent>().Subscribe(
		[](const GameObjectSelectedEvent& _event)
		{
			if (_event.entityIds.size() == 1)
			{
				Game::System<ImGuiEditor>().SelectGameObject(_event.entityIds[0]);
			}
		},
		EventScope::GLOBAL
	);
	Game::System<EventManager>().GetEvent<GameObjectCreatedEvent>().Subscribe(
		[](const GameObjectCreatedEvent& _event)
		{
			Game::System<ImGuiEditor>().SelectGameObject(_event.entityId);
		},
		EventScope::GLOBAL
	);
}

void mtgb::ImGuiEditor::Release() {}

void mtgb::ImGuiEditor::Update()
{
	pManipulator_->Update();
	if (ImGui::IsWindowFocused(ImGuiFocusedFlags_AnyWindow))
	{
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
			if (InputUtil::GetKeyDown(KeyCode::F))
			{
				// 選択中のゲームオブジェクトが一つの場合のみ接近
				auto entities = pManipulator_->GetSelectedEntityId();
				if (entities.size() == 1)
				{
					Game::System<ImGuiEditorCamera>().FrameSelected(entities[0]);
				}
			}
		}
		if (InputUtil::GetKeyDown(KeyCode::DELETE))
		{
			// マニピュレータが選択しているゲームオブジェクトを削除
			GameObjectGenerator::Delete(pManipulator_->GetSelectedEntityId());
			pManipulator_->DeselectAll();
		}
	}

	// グリッドの描画
	int interval = (static_cast<int>(gridHalfExtent_) * 2) / gridDivisionNum_;
	for (int i = 0; i <= gridDivisionNum_; i++)
	{
		float z = i * interval - gridHalfExtent_;
		{
			Vector3 s(gridHalfExtent_, 0, z);
			Vector3 e(-gridHalfExtent_, 0, z);
			MTImGui::DrawLine(s, e, 1.0f);
		}
		{
			Vector3 s(z, 0, gridHalfExtent_);
			Vector3 e(z, 0, -gridHalfExtent_);
			MTImGui::DrawLine(s, e, 1.0f);
		}
	}
}

void mtgb::ImGuiEditor::ShowImGui()
{
	std::span<EntityId> selectedEntities = pManipulator_->GetSelectedEntityId();
	if (selectedEntities.size() == 1)
	{
		ShowAddComponentDialog(selectedEntities[0]);
	}
}

void mtgb::ImGuiEditor::SaveMapData()
{
	// 編集中のファイルがない場合、新規作成する
	if (editingStagePath_.empty())
	{
		SaveMapDataAs();
	}
	// 編集中のファイルに上書き
	else
	{
		SaveStageJson(editingStagePath_);
	}
	Time::StabilizeDeltaTime();
}

void mtgb::ImGuiEditor::SaveMapDataAs()
{
	std::filesystem::path filePath(GetJsonFilePath());
	if (filePath.empty())
	{
		return;
	}
	SaveStageJson(filePath);
	// 編集中のファイルパスを変更
	editingStagePath_ = filePath;
}

void mtgb::ImGuiEditor::SaveCopyMapDataAs()
{
	std::filesystem::path filePath(GetJsonFilePath());
	if (filePath.empty())
	{
		return;
	}
	SaveStageJson(filePath);
}

void mtgb::ImGuiEditor::LoadMapData()
{
	TCHAR fileName[255] = "";
	OPENFILENAME ifn	= { 0 };

	fs::path cp = fs::current_path();
	cp.append("Stage");
	std::string str		= cp.string();
	ifn.lStructSize		= sizeof(ifn);
	ifn.hwndOwner		= WinCtxRes::GetHWND(WindowContext::FIRST);
	ifn.lpstrFilter		= "JSON File(*.json)\0*.json";
	ifn.lpstrFile		= fileName;
	ifn.nMaxFile		= 255;
	ifn.lpstrInitialDir = str.c_str();
	ifn.Flags			= OFN_NOCHANGEDIR;
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
	GameObjectGenerator::Duplicate(pManipulator_->GetSelectedEntityId());
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

void mtgb::ImGuiEditor::SelectGameObject(EntityId _entityId)
{
	EntityId selectedEntityId = _entityId;
	if (selectedEntityId == INVALID_ENTITY)
		return;

	GameObject* selectedObj = Game::System<SceneSystem>().GetActiveScene()->GetGameObject(_entityId);
	if (selectedObj == nullptr)
		return;
	inspectedObjectName_ = selectedObj->GetName();
}

void mtgb::ImGuiEditor::ShowMenuBar()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Load", "Ctrl+O"))
			{
				LoadMapData();
			}
			if (ImGui::MenuItem("Save", "Ctrl+S"))
			{
				SaveMapData();
			}
			if (ImGui::MenuItem("Save As..."))
			{
				SaveMapDataAs();
			}
			if (ImGui::MenuItem("Save Copy As.."))
			{
				SaveCopyMapDataAs();
			}
			if (ImGui::MenuItem("New Scene"))
			{
				std::filesystem::path filePath("Stage/default.json");
				if (std::filesystem::exists(filePath))
				{
					Game::System<SceneSystem>().Move<StageEditScene>(GetStageJson(filePath));
				}

				editingStagePath_.clear();
				tmpStageData_.clear();
				Game::SetEditMode(true);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("GameObject"))
		{
			auto descs = Game::System<GameObjectTypeRegistry>().GetSpawnObjectDescs();
			// 並び順取得
			auto order = Game::System<GameObjectTypeRegistry>().GetObjectPriorityOrder();

			for (size_t idx : order)
			{
				ShowGameObjectMenu(descs[idx]);
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

		if (ImGui::Button("Move To TitleScene"))
		{
			Game::System<SceneSystem>().Move<TitleScene>();
		}
		ImGui::EndMenuBar();
	}
}

void mtgb::ImGuiEditor::ShowInspector()
{
	// 複数選択されているならば、表示しない
	std::span<EntityId> entityIds = pManipulator_->GetSelectedEntityId();
	if (entityIds.size() > 1)
	{
		return;
	}
	std::list<GameObject*> gameObjects;
	Game::System<SceneSystem>().GetActiveScene()->GetAllGameObjects(&gameObjects);
	ImGui::Begin("Inspector");
	GameObject* selectedObj = nullptr;
	for (auto obj : gameObjects)
	{
		// 非表示設定されているならばスキップ
		if (obj->isInspectable_ == false)
			continue;
		bool selected = inspectedObjectName_ == obj->GetName();
		// 表示対象として記録した名前と一致する場合
		if (selected)
		{
			selectedObj			 = obj;
			inspectedObjectName_ = obj->GetName();
		}
		// クリックされた
		if (ImGui::Selectable(obj->GetName().c_str(), selected, ImGuiSelectableFlags_AllowDoubleClick))
		{
			// ダブルクリック時、対象を画面に収める
			if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				Game::System<ImGuiEditorCamera>().FrameSelected(obj->GetEntityId());
			}
			selectedObj			 = obj;
			inspectedObjectName_ = obj->GetName();
			GameObjectSelectedEvent event { .entityIds	   = { selectedObj->GetEntityId() },
											.selectionMode = SelectionMode::REPLACE };
			if (ImGui::IsKeyPressed(ImGuiKey_LeftCtrl, false))
			{
				event.selectionMode = SelectionMode::REPLACE;
			}
			Game::System<EventManager>().GetEvent<GameObjectSelectedEvent>().Invoke(event);
		}
	}
	ImGui::End();
	ImGui::Begin("Property");
	if (selectedObj != nullptr)
	{
		selectedObj->ShowImGui();
	}
	ImGui::End();
}

void mtgb::ImGuiEditor::ShowHelpMenuWindow()
{
	if (ImGui::Begin("Help") == false)
	{
		ImGui::End();
		return;
	}

	auto AddRow = [](const char* action, const char* shortcut)
	{
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::TextUnformatted(action);
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "%s", shortcut);
	};
	if (ImGui::CollapsingHeader("Operations", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginTable("table_shortcuts", 2, ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_RowBg))
		{
			ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 140.0f);
			ImGui::TableSetupColumn("Shortcut", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableHeadersRow();

			AddRow("Select", "Left Click");
			AddRow("Multi Select", "Ctrl + Left Click");
			AddRow("Rect Select", "Left Click + Drag");
			AddRow("Translate Mode", "W");
			AddRow("Rotate Mode", "R");
			AddRow("Scale Mode", "E");

			ImGui::EndTable();
		}
	}
	if (ImGui::CollapsingHeader("Shortcuts", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginTable("table_shortcuts", 2, ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_RowBg))
		{
			ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 140.0f);
			ImGui::TableSetupColumn("Shortcut", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableHeadersRow();

			AddRow("Duplicate", "Ctrl + D");
			AddRow("Delete", "Delete");
			AddRow("Undo / Redo", "Ctrl + Z / Ctrl + Y");
			AddRow("Focus Selection", "Ctrl + F or Double Click in Inspector");
			AddRow("Toggle Surface Snap", "Shift + Translate Mode");

			ImGui::EndTable();
		}
	}

	if (ImGui::CollapsingHeader("Camera Controls", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginTable("table_shortcuts", 2, ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_RowBg))
		{
			ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 140.0f);
			ImGui::TableSetupColumn("Shortcut", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableHeadersRow();

			AddRow("Pan", "Right Click + Drag");
			AddRow("Dolly", "Mouse Middle + Drag");
			AddRow("Track", "Mouse Wheel");

			ImGui::EndTable();
		}
	}

	ImGui::End();
}

void mtgb::ImGuiEditor::DrawSelectedObjectOutline()
{
	pManipulator_->DrawSelectedObjectOutline();
}

std::span<EntityId> mtgb::ImGuiEditor::GetSelectedEntityId()
{
	return pManipulator_->GetSelectedEntityId();
}

ImGuizmo::OPERATION mtgb::ImGuiEditor::GetOperation()
{
	return pManipulator_->GetOperation();
}
