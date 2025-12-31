#include "stdafx.h"
#include "ImageRenderer.h"
#include "Draw.h"
mtgb::ImageRenderer::ImageRenderer()
    : color_{Color::WHITE}
{
}
mtgb::ImageRenderer::ImageRenderer(EntityId _entityId)
    : IComponent{_entityId}
    , handle_{INVALID_HANDLE}
    , layer_{AllLayer()}
    , color_{ Color::WHITE }
{
}

void mtgb::ImageRenderer::Render() const
{
    Draw::Image(handle_, drawRect_, uiParams_,color_);
}

bool mtgb::ImageRenderer::CanRender() const
{
    return handle_ != INVALID_ENTITY;
}

GameObjectLayerFlag mtgb::ImageRenderer::GetLayer() const
{
    return layer_;
}
