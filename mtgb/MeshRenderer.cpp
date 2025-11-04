#include "MeshRenderer.h"
#include "Fbx.h"
mtgb::MeshRenderer::MeshRenderer()
{
	
}

void mtgb::MeshRenderer::OnPostRestore()
{
	meshHandle = Fbx::Load(meshFileName);
}
