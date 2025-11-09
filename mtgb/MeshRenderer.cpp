#include "MeshRenderer.h"
#include "Fbx.h"
#include "Draw.h"
#include "Transform.h"
mtgb::MeshRenderer::MeshRenderer()
{
	
}

void mtgb::MeshRenderer::Render() const
{
	Draw::FBXModel(meshHandle, Transform::Get(GetEntityId()), 0, shaderType);
}

void mtgb::MeshRenderer::OnPostRestore()
{
	meshHandle = Fbx::Load(meshFileName);
}
