#include "stdafx.h"
#include <mtgb.h>
#include "TitleScene.h"
#include "Scenes/SampleScene.h"
#include "../Source/SkySphere.h"
#include "../Source/StageManager.h"
#include "../Source/ResultScene.h"
namespace
{
	// 118,90 , 565,100
	ImageHandle hTitleImage;
	ImageHandle hBackgroundImage;
	FBXModelHandle hModel;
	RectF draw { 118, 90, 565, 100 };

	RectF textDrawRect { 118, 450, 565, 80 };
} // namespace
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
	if (InputUtil::GetKeyDown(KeyCode::DOWN) ||
		InputUtil::GetStickDown(mtgb::Axis::Y, StickType::LEFT, StickDirection::Positive))
	{
		panelManager_.MoveForcusBackward();
	}
	if (InputUtil::GetKeyDown(KeyCode::UP) ||
		InputUtil::GetStickDown(mtgb::Axis::Y, StickType::LEFT, StickDirection::Negative))
	{
		panelManager_.MoveForcusForward();
	}
	if (InputUtil::GetKeyDown(KeyCode::ENTER) || InputUtil::GetGamePadDown(PadCode::CIRCLE))
	{
		panelManager_.PressCurrentPanel();
	}
}

void TitleScene::Draw() const
{
	Draw::Image(hTitleImage, draw);
	Draw::Image(hBackgroundImage, textDrawRect, mtgb::UIParams {}, mtgb::Color(0, 0, 0, 90));
	Draw::ImmediateText("push P Key to start game...", textDrawRect);
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
		MenuItem* pItem1 = pCurrScene->Instantiate<MenuItem>();
		pItem1->SetOnPressed(
			[]()
			{
				Game::System<SceneSystem>().Move<SampleScene>();
			}
		);
		pItem1->SetText("Start Game");
		pItem1->SetRect(draw);

		MenuItem* pItem2 = pCurrScene->Instantiate<MenuItem>();
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
		pItem2->SetRect({ 118, 500, 565, 100 });

		Panel* pPanel = pCurrScene->Instantiate<Panel>();
		pPanel->AddMenuItem(pItem1);
		pPanel->AddMenuItem(pItem2);

		panelManager_.AddPanel("TitleMenu", pPanel);
	}

	{
		MenuItem* pItem1 = pCurrScene->Instantiate<MenuItem>();
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
		pPanel->AddMenuItem(pItem1);
		panelManager_.AddPanel("HowToPlay", pPanel);
	}
	panelManager_.EnablePanel("TitleMenu");
}