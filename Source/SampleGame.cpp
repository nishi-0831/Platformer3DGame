#include "SampleGame.h"
#include "Scenes/SampleScene.h"
#include "StageManager.h"
#include "TitleScene.h"
#include "StageEditScene.h"
#include "CommandHistoryManager.h"
#include "ActorManager.h"
#include <ShadowSettings.h>

using namespace mtgb;

SampleGame::SampleGame() {}

SampleGame::~SampleGame() {}

void SampleGame::SetupSystems(const RegisterSystemFuncHolder& _register)
{
	Game::Set<ComponentRegistry>(SystemUpdateType::DONT_CALL_ME);
	Game::Set<GameObjectTypeRegistry>(SystemUpdateType::DONT_CALL_ME);

	Game::Set<EntityManager>(SystemUpdateType::DONT_CALL_ME);
	Game::Set<Time>(SystemUpdateType::CYCLE);
	Game::Set<Timer>(SystemUpdateType::FRAME);

	Game::Set<RenderSystem>(SystemUpdateType::DONT_CALL_ME);
	Game::Set<AssetsManager>(SystemUpdateType::DONT_CALL_ME);

	Game::Set<Screen>(SystemUpdateType::DONT_CALL_ME);
	Game::Set<WindowManager>(SystemUpdateType::CYCLE);

	Game::Set<DirectX11Manager>(SystemUpdateType::FRAME);
	Game::Set<Direct2D>(SystemUpdateType::FRAME);
	Game::Set<DirectWrite>(SystemUpdateType::FRAME);
	Game::Set<Input>(SystemUpdateType::DONT_CALL_ME);

	Game::Set<WindowContextResourceManager>(SystemUpdateType::DONT_CALL_ME);
	Game::System<WindowContextResourceManager>()
		.RegisterResourceTypes<
			WindowResource,
			DXGIResource,
			Direct3DResource,
			Direct2DResource,
			InputResource,
			CameraResource>();

	Game::Set<SingleWindow>(SystemUpdateType::FRAME);
	Game::Set<CameraSystem>(SystemUpdateType::FRAME);

	Game::Set<ImGuiRenderer>(SystemUpdateType::FRAME);
	Game::Set<ScoreManager>(SystemUpdateType::FRAME);

	Game::Set<Debug>(SystemUpdateType::CYCLE);

	Game::Set<Image>(SystemUpdateType::DONT_CALL_ME);
	Game::Set<Audio>(SystemUpdateType::FRAME);
	Game::Set<OBJ>(SystemUpdateType::FRAME);
	Game::Set<Fbx>(SystemUpdateType::FRAME);
	Game::Set<TextCache>(SystemUpdateType::FRAME);

	Game::Set<TransformCP>(SystemUpdateType::FRAME);
	Game::Set<MeshRendererCP>(SystemUpdateType::DONT_CALL_ME);
	Game::Set<RigidBodyCP>(SystemUpdateType::FRAME);
	Game::Set<ColliderCP>(SystemUpdateType::FRAME);
	Game::Set<InterpolatorCP>(SystemUpdateType::FRAME);
	Game::Set<ImageRendererCP>(SystemUpdateType::FRAME);
	Game::Set<TextRendererCP>(SystemUpdateType::FRAME);

	Game::Set<CommandHistoryManager>(SystemUpdateType::FRAME);
	Game::Set<Draw>(SystemUpdateType::DONT_CALL_ME);
	Game::Set<EventManager>(SystemUpdateType::DONT_CALL_ME);
	Game::Set<StageManager>(SystemUpdateType::DONT_CALL_ME);
	Game::Set<ImGuiEditorCamera>(SystemUpdateType::FRAME);
	Game::Set<ImGuiEditor>(SystemUpdateType::FRAME);
	Game::Set<EffectManager>(SystemUpdateType::FRAME);
	Game::Set<ShadowSettings>(SystemUpdateType::DONT_CALL_ME);

	Game::Set<ActorManager>(SystemUpdateType::DONT_CALL_ME);
	Game::Set<SceneSystem>(SystemUpdateType::FRAME);

	Game::System<SceneSystem>().Move<StageEditScene>();
}
