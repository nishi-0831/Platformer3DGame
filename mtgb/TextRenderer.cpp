#include "stdafx.h"
#include "TextRenderer.h"
#include "Draw.h"
mtgb::TextRenderer::TextRenderer()
{
}
mtgb::TextRenderer::TextRenderer(EntityId _entityId)
	: IComponent(_entityId)
	, alignment{TextAlignment::bottomLeft}
	, text{""}
	, rect{0, 0, 0, 0}
	, fontSize{24}
	, params{.depth = 0, .layerFlag = AllLayer()}
{
}
TextRenderer& mtgb::TextRenderer::operator=(const TextRenderer& _other)
{
	if (this == &_other)
	{
		return *this;
	}
	this->alignment = _other.alignment;
	this->text		= _other.text;
	this->rect		= _other.rect;
	this->fontSize	= _other.fontSize;
	this->params	= _other.params;

	OnPostRestore();
	return *this;
}
void mtgb::TextRenderer::Render() const
{
	Draw::ImmediateText(text, rect, fontSize, alignment, params);
}

bool mtgb::TextRenderer::CanRender() const
{
	return true;
}
