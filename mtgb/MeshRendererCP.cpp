#include "MeshRendererCP.h"
void mtgb::MeshRendererCP::Initialize()
{
}

void mtgb::MeshRendererCP::Update()
{
}

void mtgb::MeshRendererCP::RenderAll() const
{
	RenderLayer(AllLayer());
}

void mtgb::MeshRendererCP::RenderLayer(GameObjectLayerFlag _layerFlag) const
{
	for (size_t i = 0; i < poolId_.size(); i++)
	{
		EntityId id = poolId_[i];

		if (poolId_[i] == INVALID_ENTITY) continue;

		const MeshRenderer& renderer = pool_[i];
		if (renderer.CanRender() == false || _layerFlag.Has(renderer.GetLayer()) == false)
			continue;
		
		renderer.Render();
	}
}
