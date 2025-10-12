#include "RenderSystem.h"
#include "WindowContextUtil.h"
#include "DirectX11Draw.h"
#include "ImGuiRenderer.h"
#include "ImGuiShowable.h"
#include "SceneSystem.h"
#include "GameObject.h"
#include "Debug.h"
#include "MTImGui.h"
#include "../ImGui/imgui.h"
#include "Draw.h"
void mtgb::RenderSystem::Initialize()
{
}

void mtgb::RenderSystem::Update()
{
}

void mtgb::RenderSystem::Render(GameScene& _scene)
{
	RenderDirectXWindows(_scene);

	RenderImGuiWindows(_scene);
}

void mtgb::RenderSystem::RenderDirectXWindows(GameScene& _scene)
{
	//��ڂ̃E�B���h�E
	WinCtxRes::ChangeResource(WindowContext::First);
	DirectX11Draw::Begin();
	DrawGameObjects(_scene, [](GameObject* pGameObject) { return pGameObject->GetLayerFlag().Has(GameObjectLayer::A); });
	Draw::FlushUIDrawCommands(GameObjectLayer::A);
	DirectX11Draw::End();
	Draw::ClearUICommands();

	//��ڂ̃E�B���h�E
	WinCtxRes::ChangeResource(WindowContext::Second);
	DirectX11Draw::Begin();
	DrawGameObjects(_scene, [](GameObject* pGameObject) { return pGameObject->GetLayerFlag().Has(GameObjectLayer::B); });
	Draw::FlushUIDrawCommands(GameObjectLayer::B);
	DirectX11Draw::End();
	Draw::ClearUICommands();

}

void mtgb::RenderSystem::RenderImGuiWindows(GameScene& _scene)
{
	using mtbit::operator|;

	// ImGui�͈�ڂ̃E�B���h�E�Ɉˑ����Ă���
	WinCtxRes::ChangeResource(WindowContext::First);

	ImGuiRenderer& imGui = Game::System<ImGuiRenderer>();

	imGui.BeginFrame();
	imGui.BeginImGuizmoFrame();

	// Inspector�\��
	MTImGui::Instance().ShowWindow(ShowType::Inspector);

	// Settings�\��
	MTImGui::Instance().ShowWindow(ShowType::Settings);

	// ���O�\��
	MTImGui::Instance().ShowLog();

	// SceneView�\��

	// RenderTargetView��ImGui�p�ɐ؂�ւ�
	imGui.SetImGuizmoRenderTargetView();

	DirectX11Draw::Begin();
	imGui.SetGameViewCamera();
	DrawGameObjects(_scene, [](GameObject* pGameObject) { return pGameObject->GetLayerFlag().Has(GameObjectLayer::A | GameObjectLayer::B | GameObjectLayer::SceneView); });

	MTImGui::Instance().ShowWindow(ShowType::SceneView);

	imGui.EndFrame();
}

void mtgb::RenderSystem::RenderGameView(GameScene& _scene)
{
	ImGuiRenderer& imGui = Game::System<ImGuiRenderer>();

}

void mtgb::RenderSystem::DrawGameObjects(GameScene& _scene, const std::function<bool(GameObject*)> _isDrawTargetCallback)
{
	_scene.Draw();
	for (auto&& gameObject : _scene.pGameObjects_)
	{
		if (_isDrawTargetCallback(gameObject))
		{
			gameObject->Draw();
		}
	}
}