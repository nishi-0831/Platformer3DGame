#include "SceneSystem.h"
#include "GameObject.h"
#include "WindowContextResourceManager.h"
#include "DirectX11Manager.h"
#include "DirectX11Draw.h"
#include "Input.h"
#include "ImGuiRenderer.h"
#include "WindowContext.h"
#include "WindowContextUtil.h"
#include "MTImGui.h"
#include "RenderSystem.h"
#include "InputData.h"
#include "WindowManager.h"
#include "CameraResource.h"
#include "InputResource.h"
#include "DXGIResource.h"
#include "Direct3DResource.h"
#include "WindowResource.h"

mtgb::SceneSystem::SceneSystem() :
	pNextScene_{ nullptr },
	onMoveListener_{}
{
}

mtgb::SceneSystem::~SceneSystem()
{
	SAFE_DELETE(GameScene::pInstance_);
}

void mtgb::SceneSystem::Initialize()
{
	OnMove([]() 
		{
			Game::System<Timer>().Clear();
		});
}

void mtgb::SceneSystem::Update()
{
	// ���̃V�[�����p�ӂ���Ă���Ȃ�V�[���`�F���W����
	if (pNextScene_)
	{
		ChangeScene();
	}

	if (GameScene::pInstance_ == nullptr)
	{
		return;  // �V�[�����Ȃ��Ȃ��A
	}

	// �X�V�A�`��O�ɃR�[���o�b�N���s
	ExecutePendingCallbacks();


	if (InputUtil::GetKeyDown(KeyCode::F11))
	{
		Game::System<WindowManager>().ChangeFullScreenState(WindowContext::First);
	}
	
	if (InputUtil::GetKeyDown(KeyCode::F1))
	{
		MTImGui::Instance().ChangeAllWindowOpen();
	}
	if (InputUtil::GetKeyDown(KeyCode::O))
	{
		Game::System<WinCtxResManager>().SwapResource<InputResource>();
	}
	
	

	if (InputUtil::GetKeyDown(KeyCode::P))
	{
		Game::System<Input>().EnumJoystick();

	}
	WinCtxRes::ChangeResource(WindowContext::First);
	Game::System<Input>().Update();
	Game::System<WindowContextResourceManager>().Update();

	

	// ���݂̃V�[��
	GameScene& currentScene{ *GameScene::pInstance_ };

	// �X�V����
	currentScene.Update();
	for (auto&& gameObject : currentScene.pGameObjects_)
	{
		gameObject->Update();
	}
	MTImGui::Instance().Update();

	// �`�揈��
	Game::System<RenderSystem>().Render(currentScene);

	// �폜����
	for (auto&& itr = currentScene.pGameObjects_.begin();
		itr != currentScene.pGameObjects_.end();)
	{
		if ((*itr)->IsToDestroy())
		{
			Game::RemoveEntityComponent((*itr)->GetEntityId());
			SAFE_DELETE(*itr);
			itr = currentScene.pGameObjects_.erase(itr);
		}
		else
		{
			itr++;
		}
	}
}

void mtgb::SceneSystem::RegisterPendingCallback(std::function<void()> _callback)
{
	pendingCallbacks_.push(_callback);
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
	// �V�[���J�ڃC�x���g�𔭓����Ă���
	for (auto& onMove : onMoveListener_)
	{
		onMove();
	}

	// �������݂̃V�[��������Ȃ�I������
	if (GameScene::pInstance_)
	{
		GameScene::pInstance_->End();
	}

	// ������ă|�C���^�ύX
	SAFE_DELETE(GameScene::pInstance_);
	GameScene::pInstance_ = pNextScene_;
	pNextScene_ = nullptr;

	// �`�F���W�����V�[���̏���������
	GameScene::pInstance_->Initialize();
}
