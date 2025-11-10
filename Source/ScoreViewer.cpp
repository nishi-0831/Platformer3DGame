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
	: pTransform_{Component<Transform>()}
	, pTextRenderer_{Component<TextRenderer>()}
{
	TextRendererData data
	{
		.rect = RectF{608,13,800,43} ,
		.fontSize = 36,
		.params = {.depth = 0,.layerFlag = AllLayer()},
		.alignment = TextAlignment::topLeft,
		.layer = AllLayer()
	};
	pTextRenderer_->text = "Score";
	pTextRenderer_->rect = data.rect;
	pTextRenderer_->fontSize = data.fontSize;
	pTextRenderer_->params = data.params;
	pTextRenderer_->alignment = data.alignment;
	pTextRenderer_->layer = data.layer;
}

ScoreViewer::ScoreViewer(RectF _rect,  int _fontSize, TextAlignment _textAlignment)
	: pTextRenderer_{ Component<TextRenderer>() }
{
	pTextRenderer_->rect = _rect;
	pTextRenderer_->fontSize = _fontSize;
	pTextRenderer_->alignment = _textAlignment;
	pTextRenderer_->params = { .depth = 0,.layerFlag = AllLayer() };
	pTextRenderer_->layer = AllLayer();

}

ScoreViewer::~ScoreViewer()
{
}

void ScoreViewer::Update()
{
	int32_t currScore = Game::System<ScoreManager>().GetScore();
	pTextRenderer_->text = std::format( "Score:{}",currScore);
}

void ScoreViewer::Draw() const
{
	//Vector2F size = Game::System<Screen>().GetSizeF();
	//std::string text = std::format("Score:{}",100);
	//Draw::ImmediateText(text, pos, 24);
}
