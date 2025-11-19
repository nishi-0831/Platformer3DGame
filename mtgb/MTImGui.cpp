#include "MTImGui.h"
#include "Transform.h"
#include "Collider.h"
#include "MeshRenderer.h"
#include "RigidBody.h"
#include "AudioPlayer.h"
#include "Vector3.h"
#include "ImGuiRenderer.h"
#include "../ImGui/imgui.h"
#include "../ImGui/ImGuizmo.h"
#include "SceneSystem.h"
#include "RectContainsInfo.h"
#include "RectDetector.h"
#include <string>
#include "GameObject.h"
#include "WindowContextUtil.h"
#include "InputResource.h"
#include "WindowResource.h"
#include "CameraSystem.h"
#include "Debug.h"
#include "ImGuiUtil.h"
#include "EventManager.h"
#include "GameObjectSelectionEvent.h"
#include "inttypes.h"
#include "../Source/MovingFloor.h"
namespace
{
    constexpr size_t BUF_SIZE = 256;
    std::string buf;
}
void mtgb::MTImGui::Initialize()
{
    buf.resize(BUF_SIZE);
    SetupShowFunc();
    Game::System<SceneSystem>().OnMove([]() 
        {
            MTImGui::Instance().showableObjs_.clear();
        });
}

void mtgb::MTImGui::Update()
{
    
    updatingImGuiShowable_ = true;

    for (ImGuiShowable* obj : showableObjs_)
    {
        DirectShow([=]()
            {
                ImGui::PushID(obj);

                obj->ShowImGui();

                ImGui::PopID();

            }, obj->displayName_, obj->show_);
    }

    updatingImGuiShowable_ = false;

	DirectShow([]()
		{
            if (ImGui::Button("EnumJoystick"))
            {
                Game::System<SceneSystem>().RegisterPendingCallback([]()
                    {
                        Game::System<Input>().EnumJoystick();

                    });
            }
		}, "Input", ShowType::Settings);
}
void mtgb::MTImGui::SetWindowOpen(ShowType _showType, bool _flag)
{
    imguiWindowStates_[_showType].isOpen = _flag;
}
void mtgb::MTImGui::SetAllWindowOpen(bool _flag)
{
    for (auto& windowState : imguiWindowStates_)
    {
        windowState.second.isOpen = _flag;
    }
}
void mtgb::MTImGui::ChangeWindowOpen(ShowType _showType)
{
    imguiWindowStates_[_showType].isOpen = !(imguiWindowStates_[_showType].isOpen);
}
void mtgb::MTImGui::ChangeAllWindowOpen()
{
    for (auto& windowState : imguiWindowStates_)
    {
        windowState.second.isOpen = !(windowState.second.isOpen);
    }
}
void mtgb::MTImGui::ShowLog()
{
    using mtgb::Debug;
    const std::list<mtgb::LogEntry>& logs = Game::System<Debug>().GetLog();

    // フィルター用のカテゴリ一覧を作成
    static std::set<std::string> availableCategories;
    static std::string selectedCategory = "All";

    // カテゴリを収集
    availableCategories.clear();
    availableCategories.insert("All");
    for (const auto& log : logs)
    {
        if (!log.category.empty())
        {
            availableCategories.insert(log.category);
        }
    }

    ImGuiRenderer& imGui = Game::System<ImGuiRenderer>();

    imGui.Begin(Debug::GetName().data());

    // カテゴリフィルター用のコンボボックス
    if (ImGui::BeginCombo("Category Filter", selectedCategory.c_str()))
    {
        for (const auto& category : availableCategories)
        {
            bool isSelected = (selectedCategory == category);
            if (ImGui::Selectable(category.c_str(), isSelected))
            {
                selectedCategory = category;
            }
            if (isSelected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    static int selectedLog = -1;
    int idx = 0;
    int displayIdx = 0;

    for (const mtgb::LogEntry& log : logs)
    {
        // フィルター適用
        if (selectedCategory != "All" && log.category != selectedCategory)
        {
            ++idx;
            continue;
        }

        std::string text = "[" + log.category + "] " + log.msg + " (" + std::to_string(log.count) + ")";

        if (ImGui::Selectable(text.c_str(), selectedLog == idx))
        {
            selectedLog = idx;
        }
        ++idx;
        ++displayIdx;
    }

    // ログの詳細表示
    if (selectedLog >= 0)
    {
        auto it = logs.begin();
        std::advance(it, selectedLog);

        ImGui::Begin("Log Details");
        ImGui::Text("Category: %s", it->category.c_str());
        ImGui::Text("File: %s", it->file.c_str());
        ImGui::Text("Line: %d", it->line);
        ImGui::Text("Function: %s", it->func.c_str());
        ImGui::End();
    }

    imGui.End();

}
mtgb::MTImGui::MTImGui()
{
    RegisterAllComponentViewers();

    // ゲームオブジェクトが選択されたときに、それを表示対象とする
    Game::System<EventManager>().GetEvent<GameObjectSelectedEvent>().Subscribe([this](const GameObjectSelectedEvent& _handler)
        {
            SelectGameObject(_handler.entityId);
        }, EventScope::Global);
    Game::System<EventManager>().GetEvent<GameObjectCreatedEvent>().Subscribe([this](const GameObjectCreatedEvent& _event)
        {
            SelectGameObject(_event.entityId);
        },EventScope::Global);
}
mtgb::MTImGui::~MTImGui()
{
    for (auto queue : showQueues_)
    {
        while (!queue.second.empty())
        {
            queue.second.pop();
        }
    }
    
    showQueues_.clear();
}
void mtgb::MTImGui::SetupShowFunc()
{
    using RegisterShowFuncHolder::Set;

    // テンプレートパラメータに型を指定
    // 第一引数に型のポインタ、第二引数に登録する型の名前

    Set<Transform>([](Transform* _target, const char* _name)
        {
            TypeRegistry::Instance().CallFunc(&_target->position, "Position");
            TypeRegistry::Instance().CallFunc(&_target->rotate, "Rotation");
            TypeRegistry::Instance().CallFunc(&_target->scale, "Scale");
        });

    Set<MeshRenderer>([](MeshRenderer* _target, const char* _name)
        {
            constexpr size_t BUF_SZ = 256; // 必要ならサイズを調整
            static std::unordered_map<EntityId, std::string> editBuffers;

            std::string& buf = editBuffers[_target->GetEntityId()];
            if (buf.empty())
            {
                // 初回は現在の値をコピー
                buf = _target->meshFileName;
            }

            // ImGuiに渡すバッファ容量を確保
            if (buf.size() < BUF_SZ) buf.resize(BUF_SZ);

            // Enterで確定されるようにフラグを指定
            if (ImGui::InputText("FileName", buf.data(), BUF_SZ, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_NoUndoRedo))
            {
                // 実際の文字列長に合わせてリサイズしてコミット
                buf.resize(std::strlen(buf.c_str()));
                _target->meshFileName = buf;
                _target->OnChangeMeshFileName();
            }
        });

    Set<Collider>([](Collider* _target, const char* _name)
        {
            ImGui::Text("Collider Property");
        });

    Set<AudioPlayer>([](AudioPlayer* _target, const char* _name)
        {
            ImGui::Text("AudioPlayer Property");
        });
   
    Set<RigidBody>([](RigidBody* _target, const char* _name)
        {
            TypeRegistry::Instance().CallFunc(&_target->velocity, "Velocity");
            TypeRegistry::Instance().CallFunc(&_target->useGravity, "UseGravity");
            TypeRegistry::Instance().CallFunc(&_target->isKinematic,"IsKinematic");
            TypeRegistry::Instance().CallFunc(&_target->isGround,"IsGround");
        });
    Set<MovingFloor>([](MovingFloor* _target, const char* _name)
        {
            TypeRegistry::Instance().CallFunc(&_target->duration, "Duration");
            TypeRegistry::Instance().CallFunc(&_target->to, "To");
            TypeRegistry::Instance().CallFunc(&_target->from, "From");
        });
    Set<ScreenCoordContainsInfo>([](ScreenCoordContainsInfo* _target, const char* _name)
        {
            TypeRegistry::Instance().CallFunc(&_target->worldPos, "WorldPos");
            ImGui::Text("ScreenPos (%.3f,%.3f)", _target->screenPos.x, _target->screenPos.y);
        
            ImGui::Text("EntityId : %lld", _target->entityId);
        });

    Set<RectDetector>([](RectDetector* _target, const char* _name)
        {
            for (auto& target : _target->GetDetectedTargets())
            {
                TypeRegistry::Instance().CallFunc(&target, "RectContains:" + target.entityId);
            }
        });

    Set<DXGI_ADAPTER_DESC1>([](DXGI_ADAPTER_DESC1* _target, const char* _name)
        {   
            // WCHARの配列を文字列に変換して表示
            char description[256];
            WideCharToMultiByte(CP_UTF8, 0, _target->Description, -1, description, sizeof(description), nullptr, nullptr);
            ImGui::LabelText("Description", "%s", description);
        });

    Set<DXGI_OUTPUT_DESC>([](DXGI_OUTPUT_DESC* _target, const char* _name)
		{
			// WCHARの配列を文字列に変換して表示
			char deviceName[64];
			WideCharToMultiByte(CP_UTF8, 0, _target->DeviceName, -1, deviceName, sizeof(deviceName), nullptr, nullptr);
			ImGui::LabelText("Device Name","%s" ,deviceName);

            ImGui::LabelText("DesktopCoordinates", "(%ld,%ld) - (%ld,%ld)", _target->DesktopCoordinates.left, _target->DesktopCoordinates.top,
                _target->DesktopCoordinates.right, _target->DesktopCoordinates.bottom);
		});
}
void mtgb::MTImGui::ShowListView(ShowType _show)
{
    auto& selectedName = imguiWindowStates_[_show].selectedName;
    auto& queue = showQueues_[_show];

    bool isSelected = false;
    std::function<void()> selectedFunc = nullptr;

    ImGui::BeginChild("List", ImVec2(200, 0), true);

    while (!queue.empty())
    {
        const std::string& name = queue.front().first;
        auto& func = queue.front().second;

        if (!isSelected)
        {
            isSelected = selectedName == name;

            // コピーキャプチャの場合は値が更新されないので、
            // 選択済みの名前と一致していたら関数を更新
            selectedFunc = func;
        }

        // 選択された項目の名前、表示関数を記録
        if (ImGui::Selectable(name.c_str(), selectedName == name))
        {
            isSelected = true;
            imguiWindowStates_[_show].selectedName = name;
            selectedFunc = func;
        }

        queue.pop();
    }
    ImGui::EndChild();

    // Listの横に property表示
    ImGui::SameLine();

    ImGui::BeginChild("property", ImVec2(0, 0), true);
    if (selectedFunc && isSelected)
    {
        selectedFunc();
    }
    ImGui::EndChild();
}
void mtgb::MTImGui::ShowComponents(EntityId _entityId)
{
    if (_entityId == INVALID_ENTITY) return;

    const auto& types = Game::System<ComponentRegistry>().GetComponentTypes(_entityId);
    if (types.has_value() == false) return;

    for (const auto& typeIdx : (*types).get())
    {
        componentShowFuncs_[typeIdx](_entityId);
    }
}
void mtgb::MTImGui::SelectGameObject(EntityId _entityId)
{
    EntityId selectedEntityId = _entityId;
    if (selectedEntityId == INVALID_ENTITY) return;

    for (ImGuiShowable* obj : showableObjs_)
    {
        if (selectedEntityId == obj->entityId_)
        {
            imguiWindowStates_[ShowType::Inspector].selectedName = obj->displayName_;
            imguiWindowStates_[ShowType::Inspector].entityId = obj->entityId_;
        }
    }
}
void mtgb::MTImGui::RegisterAllComponentViewers()
{
    RegisterComponentViewer<Transform>();
    RegisterComponentViewer<Collider>();
    RegisterComponentViewer<AudioPlayer>();
    RegisterComponentViewer<RigidBody>();
    RegisterComponentViewer<MeshRenderer>();
    RegisterComponentViewer<MovingFloor>();
}
void mtgb::MTImGui::DrawRayImpl(const Vector3& _start, const Vector3& _dir, float _thickness)
{
    Matrix4x4 proj, view;
    Game::System<CameraSystem>().GetProjMatrix(&proj);
    Game::System<CameraSystem>().GetViewMatrix(&view);
    D3D11_VIEWPORT viewport{ Game::System<mtgb::ImGuiRenderer>().GetViewport() };
    std::optional<ImVec2> p1 = ImGuiUtil::WorldToImGui(_start,proj,view,viewport);
    std::optional<ImVec2> p2 = ImGuiUtil::WorldToImGui(_start + _dir,proj,view,viewport);

    if (p1 && p2)
    {
        ImGui::GetWindowDrawList()->AddLine(p1.value(), p2.value(), IM_COL32_WHITE, _thickness);
    }
}
void mtgb::MTImGui::DrawLineImpl(const Vector3& _from, const Vector3& _to, float _thickness)
{
    Matrix4x4 proj, view;
    Game::System<CameraSystem>().GetProjMatrix(&proj);
    Game::System<CameraSystem>().GetViewMatrix(&view);
    D3D11_VIEWPORT viewport{ Game::System<mtgb::ImGuiRenderer>().GetViewport() };
    std::optional<ImVec2> p1 = ImGuiUtil::WorldToImGui(_from,proj,view,viewport);
    std::optional<ImVec2> p2 = ImGuiUtil::WorldToImGui(_to, proj, view, viewport);

    if (p1 && p2)
    {
        ImGui::GetWindowDrawList()->AddLine(p1.value(), p2.value(), IM_COL32_WHITE, _thickness);
    }
}
void mtgb::MTImGui::ShowWindow(ShowType _showType)
{
    ImGuiRenderer& imGui = Game::System<ImGuiRenderer>();
    auto& state = imguiWindowStates_[_showType];
    if (!state.isOpen)
    {
        auto& queue = showQueues_[_showType];
        while (!queue.empty())
        {
            queue.pop();
        }
    }

    if (_showType == ShowType::SceneView)
    {
        imGui.Begin(GetName(ShowType::SceneView),&state.isOpen, ImGuiRenderer::WindowFlag::NoMoveWhenHovered);
        
        imGui.RenderSceneView();
        imGui.SetDrawList();
    }
    else
    {
        imGui.Begin(GetName(_showType), &state.isOpen);
    }

    ExecuteShowQueue(_showType);

    imGui.End();
}
void mtgb::MTImGui::ExecuteShowQueue(ShowType show)
{
    if (show == ShowType::SceneView)
    {
        while (!sceneViewShowList_.empty())
        {
            sceneViewShowList_.front()();
            sceneViewShowList_.pop();
        }
    }
    else
    {
        ShowListView(show);
    }
}

void mtgb::MTImGui::Register(ImGuiShowable* obj)
{
    showableObjs_.push_back(obj);
}

void mtgb::MTImGui::Unregister(ImGuiShowable* obj)
{
    auto it = std::find(showableObjs_.begin(), showableObjs_.end(), obj);
    if (it != showableObjs_.end()) 
    {
        showableObjs_.erase(it);
    }
}



void mtgb::MTImGui::DirectShow(std::function<void()> func, const std::string& name, ShowType show)
{
    if (show == ShowType::SceneView)
    {
        // SceneViewは名前不要
        sceneViewShowList_.push(func);
    }
    else
    {
        showQueues_[show].emplace(name, func);
    }
    
}

void mtgb::MTImGui::DrawLine(const Vector3& _from, const Vector3& _to, float _thickness)
{
    if (updatingImGuiShowable_)
    {
        DrawLineImpl(_from, _to, _thickness);
    }
    else
    {
        sceneViewShowList_.push([=]() {DrawLineImpl(_from, _to, _thickness); });
    }
}

/// <summary>
/// レイを表示
/// </summary>
/// <param name="_start">レイ始点</param>
/// <param name="_dir">レイの向きと長さ</param>
/// <param name="_thickness">レイの太さ</param>
void mtgb::MTImGui::DrawVec(const Vector3& _start, const Vector3& _vec, float _thickness)
{
    if (updatingImGuiShowable_)
    {
        DrawRayImpl(_start, _vec, _thickness);
    }
    else
    {
        sceneViewShowList_.push([=]() {DrawRayImpl(_start, _vec, _thickness); });
    }
}

EntityId mtgb::MTImGui::GetSelectedEntityId()
{
    EntityId id = imguiWindowStates_[ShowType::Inspector].entityId;
    return id;
}

template<typename T>
inline void mtgb::MTImGui::RegisterComponentViewer()
{
    std::type_index typeIdx(typeid(T));

    componentShowFuncs_[typeIdx] = [this](EntityId _entityId)
        {
            GameObject* obj =  mtgb::GameObject::FindGameObject(_entityId);
            std::string name = obj->GetName() + ":Components";
            TypeRegistry::Instance().CallFunc<T>(&(T::template Get(_entityId)), name.c_str());
        };
}