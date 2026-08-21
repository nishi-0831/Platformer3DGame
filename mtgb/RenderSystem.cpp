#include "RenderSystem.h"
#include "WindowContextUtil.h"
#include "DirectX11Draw.h"
#include "ImGuiRenderer.h"
#include "ImGuiShowable.h"
#include "MTImGui.h"
#include "Draw.h"
#include "ColliderCP.h"
#include "CameraResource.h"
#include "EffekseerVFX.h"
#include "ImGuiEditor.h"
void mtgb::RenderSystem::Initialize() {}

void mtgb::RenderSystem::Update() {}

void mtgb::RenderSystem::Render(GameScene& _scene)
{
	RenderDirectXWindows(_scene);

	RenderImGuiWindows(_scene);
}

void mtgb::RenderSystem::RenderDirectXWindows(GameScene& _scene)
{
	// 一つ目のウィンドウ
	WinCtxRes::ChangeResource(WindowContext::FIRST);
	if (WinCtxRes::Get<CameraResource>(WindowContext::FIRST).NotRegisterCamera())
	{
		return;
	}

	DirectX11Draw::Begin();
	DrawGameObjects(_scene, GameObjectLayer::A);
	Game::System<mtgb::Draw>().RenderUI(GameObjectLayer::A);
	DirectX11Draw::End();
}

void mtgb::RenderSystem::RenderImGuiWindows(GameScene& _scene)
{
	using mtbit::operator|;

	// ImGuiは一つ目のウィンドウに依存している
	WinCtxRes::ChangeResource(WindowContext::FIRST);

	ImGuiRenderer& imGui = Game::System<ImGuiRenderer>();
	imGui.BeginFrame();
	imGui.BeginImGuizmoFrame();

	ImGui::Begin("DockSpace Parent", nullptr, ImGuiWindowFlags_MenuBar);

	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(dockspace_id);

	Game::System<ImGuiEditor>().ShowMenuBar();

	ImGui::End();

	// Inspector表示
	Game::System<ImGuiEditor>().ShowInspector();

	// Settings表示
	MTImGui::ShowWindow(ShowType::SETTINGS);
	MTImGui::ShowWindow(ShowType::EDITOR);
	// ログ表示
	MTImGui::ShowLog();

	//// RenderTargetViewをImGui用に切り替え
	imGui.SetImGuizmoRenderTargetView();

	DirectX11Draw::Begin();
	Game::System<ImGuiEditorCamera>().SetCamera();
	// シーン内のオブジェクト描画
	DrawGameObjects(_scene, AllLayer());
	// 選択オブジェクトのアウトライン描画
	Game::System<ImGuiEditor>().DrawSelectedObjectOutline();

	MTImGui::ShowWindow(ShowType::SCENE_VIEW);

	imGui.EndFrame();
}

void mtgb::RenderSystem::DrawGameObjects(GameScene& _scene, GameObjectLayerFlag _layer)
{
	_scene.Draw();

	std::span<IRenderableCP*> renderableCPs = Game::GetRenderableCPs();
	for (IRenderableCP* cp : renderableCPs)
	{
		cp->RenderLayer(_layer);
	}
	// TODO:EffectManager,ColliderCPもIRenderableCPインターフェースを継承させる
	Game::System<EffectManager>().Draw();
	if (_layer.Has(GameObjectLayer::SCENE_VIEW))
	{
		Game::System<ColliderCP>().Draw();
	}
}