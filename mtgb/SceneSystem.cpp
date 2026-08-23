#include "SceneSystem.h"
#include "GameObject.h"
#include "WindowContext/WindowContextResourceManager.h"
#include "Input/Input.h"
#include "WindowContext/WindowContext.h"
#include "WindowContext/WindowContextUtil.h"
#include "Editor/MTImGui.h"
#include "RenderSystem.h"
#include "Input/InputData.h"
#include "GameObjectGenerator.h"

mtgb::SceneSystem::SceneSystem()
	: pNextScene_ { nullptr }
	, onMoveListener_ {}
{
}

mtgb::SceneSystem::~SceneSystem()
{
	SAFE_DELETE(GameScene::pInstance_);
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

	if (GameScene::pInstance_ == nullptr)
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

	// 現在のシーン
	GameScene& currentScene { *GameScene::pInstance_ };

	if (Game::IsEditMode() == false)
	{
		// 更新処理
		currentScene.Update();
		for (auto&& gameObject : currentScene.pGameObjects_)
		{
			if (gameObject->IsNotCalledStart())
			{
				gameObject->Start();
				gameObject->MarkAsCalledStart();
			}
			gameObject->Update();
		}
	}

	MTImGui::Update();

	// 描画処理
	Game::System<RenderSystem>().Render(currentScene);

	MTImGui::ClearShowQueue();
	// 削除処理
	for (auto&& itr = currentScene.pGameObjects_.begin(); itr != currentScene.pGameObjects_.end();)
	{
		if ((*itr)->IsToDestroy())
		{
			Game::RemoveEntityAllComponent((*itr)->GetEntityId());
			SAFE_DELETE(*itr);
			itr = currentScene.pGameObjects_.erase(itr);
		}
		else
		{
			itr++;
		}
	}
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
	if (GameScene::pInstance_)
	{
		GameScene::pInstance_->End();
	}

	// 解放してポインタ変更
	SAFE_DELETE(GameScene::pInstance_);
	GameScene::pInstance_ = pNextScene_;
	pNextScene_			  = nullptr;

	// チェンジしたシーンの初期化処理
	GameScene::pInstance_->Initialize();
}
