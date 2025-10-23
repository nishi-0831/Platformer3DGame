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
void mtgb::MTImGui::Initialize()
{
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


    // Settings�E�B���h�E�ɕ\��
    DirectShow([]()
        {
            if (ImGui::Button("SwapWindow"))
            {
                Game::System<SceneSystem>().RegisterPendingCallback([]()
                    {
                        WinCtxRes::SwapWindow();
                        // CameraResource�͌������Ȃ�
                        Game::System<WinCtxResManager>().SwapResource<InputResource>();
                    });
			}

            if (ImGui::Button("ChangeFullscreenNearestMonitor : FirstWindow"))
            {
                Game::System<SceneSystem>().RegisterPendingCallback([]()
                    {
                        Game::System<WindowManager>().ChangeFullScreenStateNearestMonitor(WindowContext::First);
                    });
            }

            if (ImGui::Button("ChangeFullscreenNearestMonitor : SecondWindow"))
            {
                Game::System<SceneSystem>().RegisterPendingCallback([]()
                    {
                        Game::System<WindowManager>().ChangeFullScreenStateNearestMonitor(WindowContext::Second);
                    });
            }
		}, "Window", ShowType::Settings);

	DirectShow([]()
		{
            if (ImGui::Button("EnumJoystick"))
            {
                Game::System<SceneSystem>().RegisterPendingCallback([]()
                    {
                        Game::System<Input>().EnumJoystick();

                    });
            }
            if (ImGui::Button("SwapInput"))
            {
                Game::System<SceneSystem>().RegisterPendingCallback([]()
                    {
                        Game::System<WinCtxResManager>().SwapResource<InputResource>();
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

    // �t�B���^�[�p�̃J�e�S���ꗗ���쐬
    static std::set<std::string> availableCategories;
    static std::string selectedCategory = "All";

    // �J�e�S�������W
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

    // �J�e�S���t�B���^�[�p�̃R���{�{�b�N�X
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
        // �t�B���^�[�K�p
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

    // ���O�̏ڍו\��
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

    // �Q�[���I�u�W�F�N�g���I�����ꂽ�Ƃ��ɁA�����\���ΏۂƂ���
    Game::System<EventManager>().GetEvent<GameObjectSelectedEvent>().Subscribe([this](const GameObjectSelectedEvent& _handler)
        {
            EntityId selectedEntityId = _handler.entityId;
            if (selectedEntityId == INVALID_ENTITY) return;

			for (ImGuiShowable* obj : showableObjs_)
			{
                if (selectedEntityId == obj->entityId_)
                {
                    imguiWindowStates_[ShowType::Inspector].selectedName = obj->displayName_;
                    imguiWindowStates_[ShowType::Inspector].entityId = obj->entityId_;
                }
			}

        }, EventScope::Global);
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

    // �e���v���[�g�p�����[�^�Ɍ^���w��
    // �������Ɍ^�̃|�C���^�A�������ɓo�^����^�̖��O

    Set<Transform>([](Transform* _target, const char* _name)
        {
            TypeRegistry::Instance().CallFunc(&_target->position, "Position");
            TypeRegistry::Instance().CallFunc(&_target->rotate, "Rotation");
            TypeRegistry::Instance().CallFunc(&_target->scale, "Scale");
        });

    Set<MeshRenderer>([](MeshRenderer* _target, const char* _name)
        {
            ImGui::LabelText("ModelHandle", "%PRId32", _target->GetMesh());
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
            TypeRegistry::Instance().CallFunc(&_target->velocity_, "Velocity");
        });

    Set<DirectX::XMVECTOR>([](DirectX::XMVECTOR* _target, const char* _name)
        {
            ImGui::InputFloat4(_name, _target->m128_f32);
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
            // WCHAR�̔z��𕶎���ɕϊ����ĕ\��
            char description[256];
            WideCharToMultiByte(CP_UTF8, 0, _target->Description, -1, description, sizeof(description), nullptr, nullptr);
            ImGui::LabelText("Description", "%s", description);
        });

    Set<DXGI_OUTPUT_DESC>([](DXGI_OUTPUT_DESC* _target, const char* _name)
		{
			// WCHAR�̔z��𕶎���ɕϊ����ĕ\��
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

            // �R�s�[�L���v�`���̏ꍇ�͒l���X�V����Ȃ��̂ŁA
            // �I���ς݂̖��O�ƈ�v���Ă�����֐����X�V
            selectedFunc = func;
        }

        // �I�����ꂽ���ڂ̖��O�A�\���֐����L�^
        if (ImGui::Selectable(name.c_str(), selectedName == name))
        {
            isSelected = true;
            imguiWindowStates_[_show].selectedName = name;
            selectedFunc = func;
        }

        queue.pop();
    }
    ImGui::EndChild();

    // List�̉��� property�\��
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

    const auto& types = IComponentPool::GetComponentTypes(_entityId);
    if (types.has_value() == false) return;

    for (const auto& typeIdx : (*types).get())
    {
        componentShowFuncs_[typeIdx](_entityId);
    }
}
void mtgb::MTImGui::RegisterAllComponentViewers()
{
    RegisterComponentViewer<Transform>();
    RegisterComponentViewer<Collider>();
    RegisterComponentViewer<AudioPlayer>();
    RegisterComponentViewer<RigidBody>();
    RegisterComponentViewer<MeshRenderer>();
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
        // SceneView�͖��O�s�v
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
/// ���C��\��
/// </summary>
/// <param name="_start">���C�n�_</param>
/// <param name="_dir">���C�̌����ƒ���</param>
/// <param name="_thickness">���C�̑���</param>
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

template<mtgb::ComponentT T>
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