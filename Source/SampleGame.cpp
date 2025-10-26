#include "SampleGame.h"
#include "Scenes/SampleScene.h"
#pragma region �Q�[�����Œǉ�


#pragma endregion

#define TO_TEST_SCENE 1

using namespace mtgb;

SampleGame::SampleGame()
{
	
}

SampleGame::~SampleGame()
{
}

void SampleGame::SetupSystems(const RegisterSystemFuncHolder& _register)
{
	//_register(new MainWindow{}, SystemUpdateType::Cycle);

	//_register.function_
	_register.Set<EntityManager>(SystemUpdateType::DontCallMe);
	_register.Set<Time>(SystemUpdateType::Cycle);
	_register.Set<Timer>(SystemUpdateType::Frame);

	_register.Set<RenderSystem>(SystemUpdateType::DontCallMe);
	_register.Set<AssetsManager>(SystemUpdateType::DontCallMe);
	

	_register.Set<Screen>(SystemUpdateType::DontCallMe);
	_register.Set<WindowManager>(SystemUpdateType::Cycle);

	_register.Set<DirectX11Manager>(SystemUpdateType::Frame);
	_register.Set<Direct2D>(SystemUpdateType::Frame);
	_register.Set<DirectWrite>(SystemUpdateType::Frame);
	_register.Set<Input>(SystemUpdateType::DontCallMe);


	_register.Set<WindowContextResourceManager>(SystemUpdateType::DontCallMe);
	Game::System<WindowContextResourceManager>().RegisterResourceTypes<
		WindowResource,
		DXGIResource,
		Direct3DResource,
		Direct2DResource,
		InputResource,
		CameraResource
	>();

	_register.Set<SingleWindow>(SystemUpdateType::Frame);
	_register.Set<CameraSystem>(SystemUpdateType::Frame);


	_register.Set<ImGuiRenderer>(SystemUpdateType::Frame);
	_register.Set<ScoreManager>(SystemUpdateType::Frame);


	

	_register.Set<Debug>(SystemUpdateType::Cycle);

	_register.Set<Image>(SystemUpdateType::DontCallMe);
	_register.Set<Audio>(SystemUpdateType::Frame);
	_register.Set<OBJ>(SystemUpdateType::Frame);
	_register.Set<Fbx>(SystemUpdateType::Frame);
	_register.Set<Text>(SystemUpdateType::Frame);

	_register.Set<TransformCP>(SystemUpdateType::Frame, true);
	_register.Set<MeshRendererCP>(SystemUpdateType::DontCallMe, true);
	_register.Set<ColliderCP>(SystemUpdateType::Frame, true);
	_register.Set<RigidBodyCP>(SystemUpdateType::Frame, true);
	_register.Set<AudioPlayerCP>(SystemUpdateType::Frame, true);




	_register.Set<Draw>(SystemUpdateType::DontCallMe);
	_register.Set<EventManager>(SystemUpdateType::DontCallMe);
	_register.Set<ImGuiEditorCamera>(SystemUpdateType::Frame);
	_register.Set<ImGuiEditor>(SystemUpdateType::Frame);

	_register.Set<SceneSystem>(SystemUpdateType::Frame);

	// �J�n���̃V�[��
	//Game::System<SceneSystem>().Move<PlayScene>();
	//Game::System<SceneSystem>().Move<CheckTutorialScene>();
#if TO_TEST_SCENE
	Game::System<SceneSystem>().Move<SampleScene>();
#else
#endif

	//_register<MainWindow>(SystemUpdateType::Cycle);
}
