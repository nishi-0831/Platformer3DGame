#include <mtgb.h>
#include "SampleScene.h"
SampleScene::SampleScene()
{
}

SampleScene::~SampleScene()
{
}

void SampleScene::Initialize()
{
}

void SampleScene::Update()
{
	if (InputUtil::GetKeyDown(KeyCode::Escape))
	{
		Game::Exit();
	}

}

void SampleScene::Draw() const
{
}

void SampleScene::End()
{
}
