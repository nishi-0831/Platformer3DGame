#include "stdafx.h"
#include <mtgb.h>
#include "TitleScene.h"
#include "Scenes/SampleScene.h"
TitleScene::TitleScene()
{
}

TitleScene::~TitleScene()
{
}

void TitleScene::Initialize()
{

}

void TitleScene::Update()
{
	if (InputUtil::GetKeyDown(KeyCode::P))
	{
		Game::System<SceneSystem>().Move<SampleScene>();
	}
}

void TitleScene::Draw() const
{
}

void TitleScene::End()
{
}
