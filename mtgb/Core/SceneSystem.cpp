#include "Core/SceneSystem.h"
#include "Core/GameObject/GameObject.h"
#include "Window/WindowContext/WindowContextResourceManager.h"
#include "Input/Input.h"
#include "Window/WindowContext/WindowContext.h"
#include "Window/WindowContext/WindowContextUtil.h"
#include "Editor/MTImGui.h"
#include "Graphics/RenderSystem.h"
#include "Input/InputData.h"
#include "Core/GameObject/GameObjectGenerator.h"

mtgb::SceneSystem::SceneSystem()
	: pCurrentScene_ { nullptr }
	, pNextScene_ { nullptr }
	, onMoveListener_ {}
{
}

mtgb::SceneSystem::~SceneSystem()
{
	SAFE_DELETE(pCurrentScene_);
}

void mtgb::SceneSystem::Initialize()
{
	mtgb::GameObjectGenerator::Initialize();
	PropertyDisplayRegistry::Instance();
	PropertyDisplayRegistry::Instance().Initialize();
	MTImGui::Initialize();
}

void mtgb::SceneSystem::Update()
{
	// 次のシーンが用意されているならシーンチェンジする
	if (pNextScene_)
	{
		ChangeScene();
	}

	if (pCurrentScene_ == nullptr)
	{
		return; // シーンがないなら回帰
	}

	// 更新、描画前にコールバック実行
	ExecutePendingCallbacks();

	if (InputUtil::GetKeyDown(KeyCode::F1))
	{
		MTImGui::ChangeAllWindowOpen();
	}

	if (InputUtil::GetKeyDown(KeyCode::P))
	{
		Game::System<Input>().EnumJoystick();
	}
	WinCtxRes::ChangeResource(WindowContext::FIRST);
	Game::System<Input>().Update();
	Game::System<WindowContextResourceManager>().Update();

	pCurrentScene_->Update();

	MTImGui::Update();

	// 描画処理
	Game::System<RenderSystem>().Render(*pCurrentScene_);

	MTImGui::ClearShowQueue();
	pCurrentScene_->DestroyMarkedGameObjects();
}

void mtgb::SceneSystem::ExecutePendingCallbacks()
{
	while (!pendingCallbacks_.empty())
	{
		pendingCallbacks_.front()();
		pendingCallbacks_.pop();
	}
}

void mtgb::SceneSystem::ChangeScene()
{
	// 登録されたコールバックを削除
	Game::System<Timer>().Clear();

	// シーン遷移イベントを発動していく
	for (auto& onMove : onMoveListener_)
	{
		onMove();
	}

	// もし現在のシーンがあるなら終了処理
	if (pCurrentScene_)
	{
		pCurrentScene_->End();
	}

	// 解放してポインタ変更
	SAFE_DELETE(pCurrentScene_);
	pCurrentScene_ = pNextScene_;
	pNextScene_	   = nullptr;

	// チェンジしたシーンの初期化処理
	pCurrentScene_->Initialize();
}
