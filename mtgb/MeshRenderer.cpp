#include "MeshRenderer.h"
#include "Fbx.h"
#include "Draw.h"
#include "Transform.h"
namespace
{
	constexpr size_t BUFFER_SIZE{ 256 };
}
mtgb::MeshRenderer::MeshRenderer()
{
	meshFileName.resize(BUFFER_SIZE);
}

mtgb::MeshRenderer::MeshRenderer(EntityId _entityId)
	: IComponent(_entityId)
{
	meshFileName.resize(BUFFER_SIZE);
}

void mtgb::MeshRenderer::OnChangeMeshFileName()
{
	meshHandle = Fbx::Load(meshFileName);
}

void mtgb::MeshRenderer::Render() const
{
	Draw::FBXModel(meshHandle, Transform::Get(GetEntityId()), 0, shaderType);
}

void mtgb::MeshRenderer::OnPostRestore()
{
	meshHandle = Fbx::Load(meshFileName);
}
