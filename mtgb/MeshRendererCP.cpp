#include "MeshRendererCP.h"
#include "Draw.h"
#include  "Transform.h"
void mtgb::MeshRendererCP::Initialize()
{
}

void mtgb::MeshRendererCP::Update()
{
}

void mtgb::MeshRendererCP::RenderAll()
{
	/*for (size_t i = 0; i < poolId_.size(); i++)
	{
		EntityId id = poolId_[i];

		if (poolId_[i] == INVALID_ENTITY) continue;

		MeshRenderer& renderer = pool_[i];
		if (renderer.CanRender())
		{
			Draw::FBXModel(renderer.GetMesh(), Transform::Get(id), 0, renderer.GetShaderType());
		}
	}*/
	RenderLayer(AllLayer());
}

void mtgb::MeshRendererCP::RenderLayer(GameObjectLayerFlag layerFlag)
{
	for (size_t i = 0; i < poolId_.size(); i++)
	{
		EntityId id = poolId_[i];

		if (poolId_[i] == INVALID_ENTITY) continue;

		MeshRenderer& renderer = pool_[i];
		if (layerFlag.Has(renderer.GetLayer()) == false) continue;

		if (renderer.CanRender())
		{
			Draw::FBXModel(renderer.GetMesh(), Transform::Get(id), 0, renderer.GetShaderType());
		}
	}
}
