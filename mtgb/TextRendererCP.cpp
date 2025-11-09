#include "stdafx.h"
#include "TextRendererCP.h"

void mtgb::TextRendererCP::Update()
{
}

void mtgb::TextRendererCP::RenderAll() const
{
}

void mtgb::TextRendererCP::RenderLayer(GameObjectLayerFlag _layerFlag) const
{
	for (size_t i = 0; i < poolId_.size(); i++)
	{
		EntityId id = poolId_[i];

		if (poolId_[i] == INVALID_ENTITY) continue;

		const TextRenderer& renderer = pool_[i];
		//if(_layerFlag.Has(renderer.GetL))
	}
}
