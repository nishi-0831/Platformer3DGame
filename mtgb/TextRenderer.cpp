#include "stdafx.h"
#include "TextRenderer.h"
#include "Draw.h"
void mtgb::TextRenderer::Render() const
{
    Draw::ImmediateText(text, rect, fontSize, alignment, params);
}

bool mtgb::TextRenderer::CanRender() const
{
    return true;
}
