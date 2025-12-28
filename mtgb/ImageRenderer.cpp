#include "stdafx.h"
#include "ImageRenderer.h"
#include "Draw.h"
mtgb::ImageRenderer::ImageRenderer()
{
}
mtgb::ImageRenderer::ImageRenderer(EntityId _entityId)
    : IComponent{_entityId}
    , handle_{INVALID_HANDLE}
    , layer_{AllLayer()}
{
}

void mtgb::ImageRenderer::Render() const
{
    Draw::Image(handle_, drawRect_, uiParams_);
}

bool mtgb::ImageRenderer::CanRender() const
{
    return handle_ != INVALID_ENTITY;
}

GameObjectLayerFlag mtgb::ImageRenderer::GetLayer() const
{
    return layer_;
}
