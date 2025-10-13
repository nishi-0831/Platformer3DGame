#include <mtgb.h>
#include "SampleScene.h"
#include "../Source/SampleGround.h"
SampleScene::SampleScene()
{
}

SampleScene::~SampleScene()
{
}

void SampleScene::Initialize()
{
	//Instantiate<SampleGround>();
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
