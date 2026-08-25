#include "stdafx.h"
#include "ResultScene.h"
#include "TitleScene.h"
#include "SkySphere.h"
#include "StageManager.h"
#include "Button.h"

ResultScene::ResultScene() {}

ResultScene::~ResultScene() {}

void ResultScene::Initialize()
{
	Game::SetEditMode(false);

	// エディターのカメラを作成
	Game::System<ImGuiEditorCamera>().CreateCamera();

	// シーンのカメラを生成
	GameObject* pCamera = new GameObject(GameObjectBuilder().SetPosition({ 0, 0, 0 }).SetName("SceneCamera").Build());

	// ゲームオブジェクトを管理クラスに登録
	Game::System<SceneSystem>().GetActiveScene()->RegisterGameObject(pCamera);

	// スコアを表示
	RectF rect { 0, 0, 800, 600 };

	// カメラを管理クラスに登録
	CameraHandleInScene hCamera = RegisterCameraGameObject(pCamera);
	WinCtxRes::Get<CameraResource>(WindowContext::FIRST).SetHCamera(hCamera);

	// ステージをクリアしているか判定
	bool clearedStage = Game::System<StageManager>().IsClearedCurrentStage();

	std::optional<nlohmann::json> json { std::nullopt };

	// クリアしているかによって表示する画像を変える
	if (clearedStage)
	{
		Game::System<Audio>().Play("GameClear");
		json = mtgb::Game::System<StageManager>().GetStageJson(StageID::STAGE_CLEAR_SCENE);
	}
	else
	{
		Game::System<Audio>().Play("GameOver");
		json = mtgb::Game::System<StageManager>().GetStageJson(StageID::STAGE_GAME_OVER_SCENE);
	}

	if (json.has_value())
	{
		mtgb::GameObjectGenerator::GenerateFromJson(json);
		mtgb::Time::StabilizeDeltaTime();
		Game::System<CommandHistoryManager>().ClearAllStack();
	}

	Instantiate<mtgb::SkySphere>();

	CreatePanel();

	GameObject* pScoreCount = new GameObject();
	Game::System<SceneSystem>().GetActiveScene()->RegisterGameObject(pScoreCount);
	pScoreText_			   = pScoreCount->Component<TextRenderer>();
	pScoreText_->fontSize_ = 36;
	pScoreText_->alignment = TextAlignment::MIDDLE_LEFT;
	pScoreText_->rect_	   = mtgb::RectF { 400, 320, 80, 30 };
}

void ResultScene::Update()
{
	panelManager_.UpdatePanel();
}

void ResultScene::Draw() const
{
	int32_t itemCount = Game::System<ScoreManager>().GetScore();
	std::string scoreText(std::to_string(itemCount));
	pScoreText_->text_ = scoreText;
}

void ResultScene::End() {}

void ResultScene::CreatePanel()
{
	GameScene* pCurrScene = Game::System<SceneSystem>().GetActiveScene();

	std::vector<Button*> btns;
	GetGameObjects<Button>(&btns);
	auto btn = std::find_if(
		btns.begin(),
		btns.end(),
		[](Button* _pBtn)
		{
			return _pBtn->GetName() == "Button (5)";
		}
	);
	if (btn != btns.end())
	{
		(*btn)->SetOnPressed(
			[]()
			{
				Game::System<SceneSystem>().Move<TitleScene>();
			}
		);
	}

	Panel* pPanel = pCurrScene->Instantiate<Panel>();
	pPanel->AddUIComponent(*btn);

	panelManager_.AddPanel("ResultMenu", pPanel);
	panelManager_.EnablePanel("ResultMenu");
}
