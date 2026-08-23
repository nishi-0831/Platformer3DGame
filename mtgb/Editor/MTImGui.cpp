#include "MTImGui.h"
#include "Transform.h"
#include "Collider.h"
#include "MeshRenderer.h"
#include "RigidBody.h"
#include "Math/Vector3.h"
#include "ImGuiRenderer.h"
#include "ImGui/imgui.h"
#include "ImGui/ImGuizmo.h"
#include "SceneSystem.h"
#include "RectContainsInfo.h"
#include "Detector/RectDetector.h"
#include <string>
#include "WindowContext/WindowContextUtil.h"
#include "WindowContext/InputResource.h"
#include "WindowContext/WindowResource.h"
#include "CameraSystem.h"
#include "Debug.h"
#include "ImGuiUtil.h"
#include "EventManager.h"
#include "GameObjectSelectionEvent.h"

#include "../Source/MovingFloor.h"
namespace
{
	constexpr size_t BUF_SIZE = 256;
	std::string buf;
} // namespace
void mtgb::MTImGui::Initialize()
{
	buf.resize(BUF_SIZE);
	Instance().SetupShowFunc();
	Game::System<SceneSystem>().OnMove(
		[]()
		{
			auto& showableObjs = Instance().showableObjs_;
			for (auto itr = showableObjs.begin(); itr != showableObjs.end();)
			{
				if ((*itr)->scope_ == ImGuiShowable::Scope::SCENE)
				{
					itr = showableObjs.erase(itr);
				}
				else
				{
					itr++;
				}
			}
		}
	);
}

void mtgb::MTImGui::Update()
{
	Instance().updatingImGuiShowable_ = true;

	for (ImGuiShowable* obj : Instance().showableObjs_)
	{
		DirectShow(
			[=]()
			{
				ImGui::PushID(obj);

				obj->ShowImGui();

				ImGui::PopID();
			},
			obj->displayName_,
			obj->show_
		);
	}

	Instance().updatingImGuiShowable_ = false;

	DirectShow(
		[]()
		{
			if (ImGui::Button("EnumJoystick"))
			{
				Game::System<SceneSystem>().RegisterPendingCallback(
					[]()
					{
						Game::System<Input>().EnumJoystick();
					}
				);
			}
		},
		"Input",
		ShowType::SETTINGS
	);
}
void mtgb::MTImGui::SetWindowOpen(ShowType _showType, bool _flag)
{
	Instance().imguiWindowStates_[_showType].isOpen = _flag;
}
void mtgb::MTImGui::SetAllWindowOpen(bool _flag)
{
	for (auto& windowState : Instance().imguiWindowStates_)
	{
		windowState.second.isOpen = _flag;
	}
}
void mtgb::MTImGui::ChangeWindowOpen(ShowType _showType)
{
	Instance().imguiWindowStates_[_showType].isOpen = !(Instance().imguiWindowStates_[_showType].isOpen);
}
void mtgb::MTImGui::ChangeAllWindowOpen()
{
	for (auto& windowState : Instance().imguiWindowStates_)
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
	int idx				   = 0;

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
mtgb::MTImGui& mtgb::MTImGui::Instance()
{
	static MTImGui instance;
	return instance;
}
mtgb::MTImGui::MTImGui() {}
mtgb::MTImGui::~MTImGui()
{
	ClearShowQueue();

	showQueues_.clear();
}
void mtgb::MTImGui::SetupShowFunc()
{
	using RegisterShowFuncHolder::Set;

	// テンプレートパラメータに型を指定
	// 第一引数に型のポインタ、第二引数に登録する型の名前
	Set<ScreenCoordContainsInfo>(
		[](ScreenCoordContainsInfo* _target, const char* _name)
		{
			PropertyDisplayRegistry::Instance().ShowProperty(&_target->worldPos, "WorldPos");
			ImGui::Text("ScreenPos (%.3f,%.3f)", _target->screenPos.x, _target->screenPos.y);

			ImGui::Text("EntityId : %lld", _target->entityId);
		}
	);

	Set<RectDetector>(
		[](RectDetector* _target, const char* _name)
		{
			for (auto& target : _target->GetDetectedTargets())
			{
				PropertyDisplayRegistry::Instance().ShowProperty(
					&target,
					std::format("RectContains:{}", target.entityId).c_str()
				);
			}
		}
	);

	Set<DXGI_ADAPTER_DESC1>(
		[](DXGI_ADAPTER_DESC1* _target, const char* _name)
		{
			// WCHARの配列を文字列に変換して表示
			char description[256];
			WideCharToMultiByte(
				CP_UTF8,
				0,
				_target->Description,
				-1,
				description,
				sizeof(description),
				nullptr,
				nullptr
			);
			ImGui::LabelText("Description", "%s", description);
		}
	);

	Set<DXGI_OUTPUT_DESC>(
		[](DXGI_OUTPUT_DESC* _target, const char* _name)
		{
			// WCHARの配列を文字列に変換して表示
			char deviceName[64];
			WideCharToMultiByte(CP_UTF8, 0, _target->DeviceName, -1, deviceName, sizeof(deviceName), nullptr, nullptr);
			ImGui::LabelText("Device Name", "%s", deviceName);

			ImGui::LabelText(
				"DesktopCoordinates",
				"(%ld,%ld) - (%ld,%ld)",
				_target->DesktopCoordinates.left,
				_target->DesktopCoordinates.top,
				_target->DesktopCoordinates.right,
				_target->DesktopCoordinates.bottom
			);
		}
	);
	Set<Color>(
		[](Color* _target, const char* _name)
		{
			Vector4 vec4 = _target->ToVector4Norm();
			ImGui::ColorEdit4(_name, vec4.f, ImGuiColorEditFlags_Uint8);
		}
	);
}
void mtgb::MTImGui::ClearShowQueue()
{
	for (auto& queue : Instance().showQueues_)
	{
		while (queue.second.empty() == false)
		{
			queue.second.pop();
		}
	}
}
void mtgb::MTImGui::ShowListView(ShowType _show)
{
	ImGui::BeginChild("List", ImVec2(200, 0), true);
	std::function<void()> selectedFunc = GetSelectedFunc(_show);
	ImGui::EndChild();
	// Listの横に property表示
	ImGui::SameLine();
	if (selectedFunc != nullptr)
	{
		ImGui::BeginChild("property", ImVec2(0, 0), true);
		selectedFunc();
		ImGui::EndChild();
	}
}
std::function<void()> mtgb::MTImGui::GetSelectedFunc(ShowType _show)
{
	auto& selectedName = imguiWindowStates_[_show].selectedName;
	auto& queue		   = showQueues_[_show];

	bool isSelected					   = false;
	std::function<void()> selectedFunc = nullptr;

	while (!queue.empty())
	{
		const std::string& name = queue.front().first;
		auto& func				= queue.front().second;

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
			isSelected							   = true;
			imguiWindowStates_[_show].selectedName = name;
			selectedFunc						   = func;
		}

		queue.pop();
	}
	if (selectedFunc && isSelected)
	{
		return selectedFunc;
	}
	return nullptr;
}
void mtgb::MTImGui::ShowComponents(EntityId _entityId)
{
	if (_entityId == INVALID_ENTITY)
		return;

	const auto& types = Game::System<ComponentRegistry>().GetComponentTypes(_entityId);
	if (types.has_value() == false)
		return;

	for (const auto& typeIdx : (*types).get())
	{
		Instance().componentShowFuncs_[typeIdx](_entityId);
	}
}

void mtgb::MTImGui::DrawRayImpl(const Vector3& _start, const Vector3& _dir, float _thickness)
{
	Matrix4x4 proj, view;
	CameraSystem& cameraSystem = Game::System<CameraSystem>();
	cameraSystem.GetProjMatrix(&proj);
	cameraSystem.GetViewMatrix(&view);
	D3D11_VIEWPORT viewport { Game::System<mtgb::ImGuiRenderer>().GetViewport() };
	float nearZ = cameraSystem.GetNear();
	float farZ	= cameraSystem.GetFar();
	auto result = ImGuiUtil::WorldToImGuiClipped(_start, _start + _dir, proj, view, viewport, nearZ, farZ);

	if (result)
	{
		ImGui::GetWindowDrawList()->AddLine(result->first, result->second, IM_COL32_WHITE, _thickness);
	}
}
void mtgb::MTImGui::DrawLineImpl(const Vector3& _from, const Vector3& _to, float _thickness)
{
	Matrix4x4 proj, view;
	CameraSystem& cameraSystem = Game::System<CameraSystem>();
	cameraSystem.GetProjMatrix(&proj);
	cameraSystem.GetViewMatrix(&view);
	D3D11_VIEWPORT viewport { Game::System<mtgb::ImGuiRenderer>().GetViewport() };
	float nearZ = cameraSystem.GetNear();
	float farZ	= cameraSystem.GetFar();
	auto result = ImGuiUtil::WorldToImGuiClipped(_from, _to, proj, view, viewport, nearZ, farZ);

	if (result)
	{
		ImGui::GetWindowDrawList()->AddLine(result->first, result->second, IM_COL32_WHITE, _thickness);
	}
}
void mtgb::MTImGui::ShowWindow(ShowType _showType)
{
	ImGuiRenderer& imGui = Game::System<ImGuiRenderer>();
	auto& state			 = Instance().imguiWindowStates_[_showType];
	if (!state.isOpen)
	{
		auto& queue = Instance().showQueues_[_showType];
		while (!queue.empty())
		{
			queue.pop();
		}
	}

	if (_showType == ShowType::SCENE_VIEW)
	{
		imGui.Begin(
			GetName(ShowType::SCENE_VIEW),
			&state.isOpen,
			ImGuiRenderer::WindowFlag::NO_MOVE_WHEN_HOVERED | ImGuiRenderer::WindowFlag::NO_SCROLL
		);

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
void mtgb::MTImGui::ExecuteShowQueue(ShowType _show)
{
	if (_show == ShowType::SCENE_VIEW)
	{
		while (Instance().sceneViewShowList_.empty() == false)
		{
			Instance().sceneViewShowList_.front()();
			Instance().sceneViewShowList_.pop();
		}
	}
	else
	{
		Instance().ShowListView(_show);
	}
}

void mtgb::MTImGui::Register(ImGuiShowable* _obj)
{
	auto it = std::find(Instance().showableObjs_.begin(), Instance().showableObjs_.end(), _obj);
	if (it == Instance().showableObjs_.end())
	{
		Instance().showableObjs_.push_back(_obj);
	}
}

void mtgb::MTImGui::Unregister(ImGuiShowable* _obj)
{
	auto it = std::find(Instance().showableObjs_.begin(), Instance().showableObjs_.end(), _obj);
	if (it != Instance().showableObjs_.end())
	{
		Instance().showableObjs_.erase(it);
	}
}

void mtgb::MTImGui::DrawLine(const Vector3& _from, const Vector3& _to, float _thickness)
{
	if (Instance().updatingImGuiShowable_)
	{
		Instance().DrawLineImpl(_from, _to, _thickness);
	}
	else
	{
		Instance().sceneViewShowList_.push(
			[_from, _to, _thickness]()
			{
				Instance().DrawLineImpl(_from, _to, _thickness);
			}
		);
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
	if (Instance().updatingImGuiShowable_)
	{
		Instance().DrawRayImpl(_start, _vec, _thickness);
	}
	else
	{
		Instance().sceneViewShowList_.push(
			[_start, _vec, _thickness]()
			{
				Instance().DrawRayImpl(_start, _vec, _thickness);
			}
		);
	}
}

void mtgb::MTImGui::DrawCone(
	const Vector3& _position,
	const Vector3& _direction,
	float _fovAngleDegree,
	float _distance,
	float _thickness,
	int _segments
)
{
	// 正規化された方向ベクトル
	Vector3 forward = Vector3::Normalize(_direction);

	// 円錐の底面の中心
	Vector3 baseCenter = _position + forward * _distance;

	// 円錐の底面の半径
	// MEMO: tanθ = 対辺 / 底辺 : 対辺 = 底辺 * tanθ
	float baseRadius = _distance * std::tanf(DirectX::XMConvertToRadians(_fovAngleDegree / 2.0f));

	// 上下方向を決定
	Vector3 up =
		std::fabsf(DirectX::XMVectorGetY(forward)) < 0.9f ? Vector3 { 0.0f, 1.0f, 0.0f } : Vector3 { 1.0f, 0.0f, 0.0f };

	// 右方向を計算
	Vector3 right = Vector3::Normalize(Vector3::Cross(forward, up));

	// 上方向を再計算
	up = Vector3::Cross(right, forward);

	// 底面の円周上の点を計算
	std::vector<Vector3> baseCirclePoints(_segments);
	for (int i = 0; i < _segments; i++)
	{
		float angle			= DirectX::XM_2PI * i / _segments;
		float x				= baseRadius * std::cosf(angle);
		float y				= baseRadius * std::sinf(angle);
		baseCirclePoints[i] = baseCenter + right * x + up * y;
	}

	// 円錐の側面を描画
	for (int i = 0; i < _segments; i++)
	{
		DrawLine(_position, baseCirclePoints[i], _thickness);
	}

	// 底面の円を描画
	for (int i = 0; i < _segments; i++)
	{
		int nextIdx = (i + 1) % _segments;
		DrawLine(baseCirclePoints[i], baseCirclePoints[nextIdx], _thickness);
	}
}
