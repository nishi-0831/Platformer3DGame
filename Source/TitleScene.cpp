#include "stdafx.h"
#include <mtgb.h>
#include "TitleScene.h"
#include "Scenes/SampleScene.h"
#include "../Source/SkySphere.h"
#include "../Source/StageManager.h"
#include "../Source/PanelManager.h"
#include "../Source/ResultScene.h"
namespace
{
	// 118,90 , 565,100
	ImageHandle hTitleImage;
	ImageHandle hBackgroundImage;
	FBXModelHandle hModel;
	RectF draw { 118, 90, 565, 100 };
	
	RectF draw2 { 236, 90, 665, 100 };
	RectF textDrawRect { 118, 450, 565, 80 };
	PanelManager panelManager;
} // namespace
void CreatePanel();
TitleScene::TitleScene() {}

TitleScene::~TitleScene() {}

void TitleScene::Initialize()
{
	Game::System<ImGuiEditorCamera>().CreateCamera();

	GameObject* pCamera = new GameObject(GameObjectBuilder().SetPosition({ 0, 0, 0 }).SetName("SceneCamera").Build());
	Game::System<SceneSystem>().GetActiveScene()->RegisterGameObject(pCamera);
	hTitleImage		 = Image::Load("Image/TitleImage.png");
	hBackgroundImage = Image::Load("Image/Black.png");

	CameraHandleInScene hCamera = RegisterCameraGameObject(pCamera);
	WinCtxRes::Get<CameraResource>(WindowContext::FIRST).SetHCamera(hCamera);

	std::optional<nlohmann::json> json = mtgb::Game::System<StageManager>().GetStageJson(StageID::STAGE_TITLE_SCENE);
	if (json.has_value())
	{
		mtgb::GameObjectGenerator::GenerateFromJson(json);
		mtgb::Time::StabilizeDeltaTime();
	}
	Instantiate<mtgb::SkySphere>();

	CreatePanel();
}

void TitleScene::Update()
{
	if (InputUtil::GetKeyDown(KeyCode::DOWN))
	{
		panelManager.MoveForcusBackward();
	}
	if (InputUtil::GetKeyDown(KeyCode::UP))
	{
		panelManager.MoveForcusForward();
	}
	if (InputUtil::GetKeyDown(KeyCode::ENTER))
	{
		panelManager.PressCurrentPanel();
	}
}

void TitleScene::Draw() const
{
	Draw::Image(hTitleImage, draw);
	Draw::Image(hBackgroundImage, textDrawRect, mtgb::UIParams {}, mtgb::Color(0, 0, 0, 90));
	Draw::ImmediateText("push P Key to start game...", textDrawRect);
}

void TitleScene::End() {}

void CreatePanel()
{
	GameScene* pCurrScene = Game::System<SceneSystem>().GetActiveScene();
	GameObject* howToPlayImageObj = new GameObject(GameObjectBuilder().SetName("HowToPlayImage").Build());
	pCurrScene->RegisterGameObject(howToPlayImageObj);
	ImageRenderer* pImgRenderer = howToPlayImageObj->Component<ImageRenderer>();
	pImgRenderer->handle_		  = Image::Load("Image/HowToPlay.png");
	pImgRenderer->drawRect_		  = RectF { 0, 0, 800, 700 };
	pImgRenderer->enabled_		  = false;

	{
		MenuItem* pItem1 = pCurrScene->Instantiate<MenuItem>();
		pItem1->SetOnPressed(
			[]()
			{
				Game::System<SceneSystem>().Move<SampleScene>();
			}
		);
		pItem1->SetText("Start Game");
		RectF rect1 { 300, 300, 100, 100 };
		pItem1->SetRect(draw);
	
		MenuItem* pItem2 = pCurrScene->Instantiate<MenuItem>();
		pItem2->SetOnPressed(
			[]()
			{
				GameObject* pGameObj = Game::System<SceneSystem>().GetActiveScene()->GetGameObject("HowToPlayImage");
				if (pGameObj == nullptr)
					return;

				pGameObj->Component<ImageRenderer>()->enabled_ = true;
				panelManager.EnablePanel("HowToPlay");
			}
		);
		pItem2->SetText("How to Play");
		RectF rect2 { 500, 500, 100, 100 };
		pItem2->SetRect(rect2);

		Panel* pPanel = pCurrScene->Instantiate<Panel>();
		pPanel->AddMenuItem(pItem1);
		pPanel->AddMenuItem(pItem2);

		panelManager.AddPanel("TitleMenu", pPanel);
	}

	{
		MenuItem* pItem1 = pCurrScene->Instantiate<MenuItem>();
		pItem1->SetOnPressed(
			[]()
			{
				GameObject* pGameObj = Game::System<SceneSystem>().GetActiveScene()->GetGameObject("HowToPlayImage");
				if (pGameObj == nullptr)
					return;

				pGameObj->Component<ImageRenderer>()->enabled_ = false;
				panelManager.EnablePanel("TitleMenu");		
			}
		);
		pItem1->SetText("Close");
		RectF rect1 { 300, 500, 100, 100 };
		pItem1->SetRect(rect1);
	
		Panel* pPanel = pCurrScene->Instantiate<Panel>();
		pPanel->AddMenuItem(pItem1);
		panelManager.AddPanel("HowToPlay", pPanel);

	}
		panelManager.EnablePanel("TitleMenu");
}