#include "stdafx.h"
#include "TextRenderer.h"
#include "Draw.h"
mtgb::TextRenderer::TextRenderer() {}
mtgb::TextRenderer::TextRenderer(EntityId _entityId)
	: IComponent(_entityId)
	, alignment { TextAlignment::BOTTOM_LEFT }
	, text_ { "" }
	, rect_ { 0, 0, 0, 0 }
	, fontSize_ { 24 }
	, params_ { .depth = 0, .layerFlag = AllLayer() }
{
}

void mtgb::TextRenderer::Render() const
{
	Draw::ImmediateText(text_, rect_, fontSize_, alignment, params_);
}

bool mtgb::TextRenderer::CanRender() const
{
	return true;
}
