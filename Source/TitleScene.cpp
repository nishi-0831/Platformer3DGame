#include "stdafx.h"
#include <mtgb.h>
#include <ProfileUtlity.h>
#include "TitleScene.h"
#include "Scenes/SampleScene.h"
#include "../Source/SkySphere.h"
#include "../Source/StageManager.h"
#include "../Source/ResultScene.h"
#include "Slider.h"
#include "Button.h"
#include <SerializableGameObject.h>

TitleScene::TitleScene() {}

TitleScene::~TitleScene() {}

void TitleScene::Initialize()
{
	Game::System<ImGuiEditorCamera>().CreateCamera();
	Game::System<Audio>().Play("TitleScene", true);
	Game::System<SceneSystem>().OnMove(
		[]()
		{
			Game::System<Audio>().Stop("TitleScene");
		}
	);
	GameObject* pCamera = new GameObject(GameObjectBuilder().SetPosition({ 0, 0, 0 }).SetName("SceneCamera").Build());
	Game::System<SceneSystem>().GetActiveScene()->RegisterGameObject(pCamera);
	
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
	panelManager_.UpdatePanel();
}

void TitleScene::Draw() const
{
}

void TitleScene::End() {}

void TitleScene::CreatePanel()
{
	GameScene* pCurrScene		  = Game::System<SceneSystem>().GetActiveScene();
	GameObject* howToPlayImageObj = new GameObject(GameObjectBuilder().SetName("HowToPlayImage").Build());
	pCurrScene->RegisterGameObject(howToPlayImageObj);
	ImageRenderer* pImgRenderer = howToPlayImageObj->Component<ImageRenderer>();
	pImgRenderer->handle_		= Image::Load("Image/HowToPlay.png");
	pImgRenderer->enabled_		= false;

	Vector2F screenSize		= Game::System<Screen>().GetSizeF();
	pImgRenderer->drawRect_ = RectF { 0, 0, screenSize.x, 500 };

	{
		Button* pItem1 = pCurrScene->Instantiate<Button>();
		pItem1->SetOnPressed(
			[]()
			{
				Game::System<SceneSystem>().Move<SampleScene>();
			}
		);
		pItem1->SetText("Start Game");
		pItem1->SetRect({ 290, 300, 220, 60 });

		Button* pItem2 = pCurrScene->Instantiate<Button>();
		pItem2->SetOnPressed(
			[this]()
			{
				GameObject* pGameObj = Game::System<SceneSystem>().GetActiveScene()->GetGameObject("HowToPlayImage");
				if (pGameObj == nullptr)
					return;

				pGameObj->Component<ImageRenderer>()->enabled_ = true;
				panelManager_.EnablePanel("HowToPlay");
			}
		);
		pItem2->SetText("How to Play");
		pItem2->SetRect({ 290, 430, 220, 60 });

		Button* pItem3 = pCurrScene->Instantiate<Button>();
		pItem3->SetOnPressed(
			[this]()
			{
				panelManager_.EnablePanel("Setting");
			}
		);
		pItem3->SetText("Setting");
		pItem3->SetRect({ 290, 530, 220, 60 });

		Panel* pPanel = pCurrScene->Instantiate<Panel>();
		pPanel->AddUIComponent(pItem1);
		pPanel->AddUIComponent(pItem2);
		pPanel->AddUIComponent(pItem3);

		panelManager_.AddPanel("TitleMenu", pPanel);
	}

	// Setting
	{

		Slider* pSlider = pCurrScene->Instantiate<Slider>();

		pSlider->SetRect({ 110, 400, 300, 50 });
		pSlider->SetLabel("Slider");
		pSlider->SetOnValueChanged(
			[](int _value)
			{
				ProfileInt::Load().Section("Game").Param("CameraSpeed").Write(_value);
			}
		);
		pSlider->SetValue(ProfileInt::Load().Section("GAME").Param("CameraSpeed").InitValue(60).Get());

		Button* pItem1 = pCurrScene->Instantiate<Button>();
		pItem1->SetOnPressed(
			[this]()
			{
				panelManager_.EnablePanel("TitleMenu");
			}
		);
		pItem1->SetText("Close");
		pItem1->SetRect({ 118, 500, 565, 100 });
		Panel* pPanel = pCurrScene->Instantiate<Panel>();

		pPanel->AddUIComponent(pSlider);
		pPanel->AddUIComponent(pItem1);
		panelManager_.AddPanel("Setting", pPanel);
	}

	// HowToPlay
	{
		Button* pItem1 = pCurrScene->Instantiate<Button>();
		pItem1->SetOnPressed(
			[this]()
			{
				GameObject* pGameObj = Game::System<SceneSystem>().GetActiveScene()->GetGameObject("HowToPlayImage");
				if (pGameObj == nullptr)
					return;

				pGameObj->Component<ImageRenderer>()->enabled_ = false;
				panelManager_.EnablePanel("TitleMenu");
			}
		);
		pItem1->SetText("Close");
		pItem1->SetRect({ 118, 500, 565, 100 });

		Panel* pPanel = pCurrScene->Instantiate<Panel>();
		pPanel->AddUIComponent(pItem1);
		panelManager_.AddPanel("HowToPlay", pPanel);
	}

	panelManager_.EnablePanel("TitleMenu");
}