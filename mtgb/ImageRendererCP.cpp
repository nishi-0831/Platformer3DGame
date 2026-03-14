#include "stdafx.h"
#include "ImageRendererCP.h"

void mtgb::ImageRendererCP::Update() {}

void mtgb::ImageRendererCP::RenderAll() const
{
	RenderLayer(AllLayer());
}

void mtgb::ImageRendererCP::RenderLayer(GameObjectLayerFlag _layerFlag) const
{
	for (size_t i = 0; i < poolId_.size(); i++)
	{
		EntityId id = poolId_[i];

		if (poolId_[i] == INVALID_ENTITY)
			continue;

		const ImageRenderer& renderer = pool_[i];
		if (renderer.CanRender() && _layerFlag.Has(renderer.GetLayer()))
		{
			renderer.Render();
		}
	}
}
