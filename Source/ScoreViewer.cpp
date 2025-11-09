#include "stdafx.h"
#include "ScoreViewer.h"
#include <format>
namespace
{
	// 608,13
	Vector2F pos{ 608,13 };
	TextAlignment alignment{ TextAlignment::topLeft };
}
ScoreViewer::ScoreViewer()
{
}

ScoreViewer::~ScoreViewer()
{
}

void ScoreViewer::Update()
{
}

void ScoreViewer::Draw() const
{
	Vector2F size = Game::System<Screen>().GetSizeF();
	std::string text = std::format("Score:{}",100);
	Draw::ImmediateText(text, pos, 24);
}
