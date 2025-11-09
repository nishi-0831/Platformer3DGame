#include "SampleGame.h"
#include "Scenes/SampleScene.h"
#pragma region ゲーム側で追加


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
	Game::Set<ComponentRegistry>(SystemUpdateType::DontCallMe);
	Game::Set<GameObjectTypeRegistry>(SystemUpdateType::DontCallMe);

	Game::Set<EntityManager>(SystemUpdateType::DontCallMe);
	Game::Set<Time>(SystemUpdateType::Cycle);
	Game::Set<Timer>(SystemUpdateType::Frame);

	Game::Set<RenderSystem>(SystemUpdateType::DontCallMe);
	Game::Set<AssetsManager>(SystemUpdateType::DontCallMe);
	

	Game::Set<Screen>(SystemUpdateType::DontCallMe);
	Game::Set<WindowManager>(SystemUpdateType::Cycle);

	Game::Set<DirectX11Manager>(SystemUpdateType::Frame);
	Game::Set<Direct2D>(SystemUpdateType::Frame);
	Game::Set<DirectWrite>(SystemUpdateType::Frame);
	Game::Set<Input>(SystemUpdateType::DontCallMe);


	Game::Set<WindowContextResourceManager>(SystemUpdateType::DontCallMe);
	Game::System<WindowContextResourceManager>().RegisterResourceTypes<
		WindowResource,
		DXGIResource,
		Direct3DResource,
		Direct2DResource,
		InputResource,
		CameraResource
	>();

	Game::Set<SingleWindow>(SystemUpdateType::Frame);
	Game::Set<CameraSystem>(SystemUpdateType::Frame);


	Game::Set<ImGuiRenderer>(SystemUpdateType::Frame);
	Game::Set<ScoreManager>(SystemUpdateType::Frame);


	

	Game::Set<Debug>(SystemUpdateType::Cycle);

	Game::Set<Image>(SystemUpdateType::DontCallMe);
	Game::Set<Audio>(SystemUpdateType::Frame);
	Game::Set<OBJ>(SystemUpdateType::Frame);
	Game::Set<Fbx>(SystemUpdateType::Frame);
	Game::Set<TextCache>(SystemUpdateType::Frame);

	Game::Set<TransformCP>(SystemUpdateType::Frame);
	Game::Set<MeshRendererCP>(SystemUpdateType::DontCallMe);
	Game::Set<RigidBodyCP>(SystemUpdateType::Frame);
	Game::Set<ColliderCP>(SystemUpdateType::Frame);
	Game::Set<AudioPlayerCP>(SystemUpdateType::Frame);




	Game::Set<Draw>(SystemUpdateType::DontCallMe);
	Game::Set<EventManager>(SystemUpdateType::DontCallMe);
	Game::Set<ImGuiEditorCamera>(SystemUpdateType::Frame);
	Game::Set<ImGuiEditor>(SystemUpdateType::Frame);

	Game::Set<SceneSystem>(SystemUpdateType::Frame);

	// 開始時のシーン
	//Game::System<SceneSystem>().Move<PlayScene>();
	//Game::System<SceneSystem>().Move<CheckTutorialScene>();
#if TO_TEST_SCENE
	Game::System<SceneSystem>().Move<SampleScene>();
#else
#endif

	//_register<MainWindow>(SystemUpdateType::Cycle);
}
