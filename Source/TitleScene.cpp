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
	Game::SetEditMode(false);
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

void TitleScene::Draw() const {}

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
	std::vector<Button*> btns;
	GetGameObjects<Button>(&btns);
	auto pBtn1 = std::find_if(
		btns.begin(),
		btns.end(),
		[](Button* _pBtn)
		{
			return _pBtn->GetName() == "Button (0)";
		}
	);
	(*pBtn1)->SetOnPressed(
		[]()
		{
			Game::System<SceneSystem>().Move<SampleScene>();
		}
	);
	auto pBtn2 = std::find_if(
		btns.begin(),
		btns.end(),
		[](Button* _pBtn)
		{
			return _pBtn->GetName() == "Button (1)";
		}
	);
	(*pBtn2)->SetOnPressed(
		[this]()
		{
			GameObject* pGameObj = Game::System<SceneSystem>().GetActiveScene()->GetGameObject("HowToPlayImage");
			if (pGameObj == nullptr)
				return;

			pGameObj->Component<ImageRenderer>()->enabled_ = true;
			panelManager_.EnablePanel("HowToPlay");
		}
	);
	auto pBtn3 = std::find_if(
		btns.begin(),
		btns.end(),
		[](Button* _pBtn)
		{
			return _pBtn->GetName() == "Button (2)";
		}
	);
	(*pBtn3)->SetOnPressed(
		[this]()
		{
			panelManager_.EnablePanel("Setting");
		}
	);

	auto pBtn4 = std::find_if(
		btns.begin(),
		btns.end(),
		[](Button* _pBtn)
		{
			return _pBtn->GetName() == "Button (3)";
		}
	);
	(*pBtn4)->SetOnPressed(
		[this]()
		{
			panelManager_.EnablePanel("TitleMenu");
		}
	);

	auto pBtn5 = std::find_if(
		btns.begin(),
		btns.end(),
		[](Button* _pBtn)
		{
			return _pBtn->GetName() == "Button (4)";
		}
	);
	(*pBtn5)->SetOnPressed(
		[this]()
		{
			GameObject* pGameObj = Game::System<SceneSystem>().GetActiveScene()->GetGameObject("HowToPlayImage");
			if (pGameObj == nullptr)
				return;

			pGameObj->Component<ImageRenderer>()->enabled_ = false;
			panelManager_.EnablePanel("TitleMenu");
		}
	);

	std::vector<Slider*> sliders;
	GetGameObjects<Slider>(&sliders);
	auto pSlider1 = std::find_if(
		sliders.begin(),
		sliders.end(),
		[](Slider* _pSlider)
		{
			return _pSlider->GetName() == "Slider (0)";
		}
	);
	(*pSlider1)->SetOnValueChanged(
		[](int _value)
		{
			ProfileInt::Load().Section("Game").Param("CameraSpeed").Write(_value);
		}
	);
	(*pSlider1)->SetValue(ProfileInt::Load().Section("GAME").Param("CameraSpeed").InitValue(60).Get());

	{

		Panel* pPanel = pCurrScene->Instantiate<Panel>();
		pPanel->AddUIComponent(*pBtn1);
		pPanel->AddUIComponent(*pBtn2);
		pPanel->AddUIComponent(*pBtn3);

		panelManager_.AddPanel("TitleMenu", pPanel);
	}

	// Setting
	{
		Panel* pPanel = pCurrScene->Instantiate<Panel>();

		pPanel->AddUIComponent(*pSlider1);
		pPanel->AddUIComponent(*pBtn4);
		panelManager_.AddPanel("Setting", pPanel);
	}

	// HowToPlay
	{
		Panel* pPanel = pCurrScene->Instantiate<Panel>();
		pPanel->AddUIComponent(*pBtn5);
		panelManager_.AddPanel("HowToPlay", pPanel);
	}

	panelManager_.EnablePanel("TitleMenu");
}