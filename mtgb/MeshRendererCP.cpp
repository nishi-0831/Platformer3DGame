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
	for (MeshRenderer& renderer : pool_)
	{
		EntityId id = renderer.GetEntityId();
		if (id == INVALID_ENTITY) continue;
		if (renderer.CanRender())
		{
			Draw::FBXModel(renderer.GetMesh(), Transform::Get(id), 0,renderer.GetShaderType());
		}
	}
}

void mtgb::MeshRendererCP::RenderLayer(GameObjectLayerFlag layerFlag)
{
	for (MeshRenderer& renderer : pool_)
	{
		EntityId id = renderer.GetEntityId();

		if (id == INVALID_ENTITY) continue;
		if (layerFlag.Has(renderer.GetLayer()) == false) continue;

		if (renderer.CanRender())
		{
			Draw::FBXModel(renderer.GetMesh(), Transform::Get(id), 0, renderer.GetShaderType());
		}
	}
}
