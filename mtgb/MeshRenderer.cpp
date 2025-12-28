#include "MeshRenderer.h"
#include "Fbx.h"
#include "Draw.h"
#include "Transform.h"
namespace
{
	constexpr size_t BUFFER_SIZE{ 256 };
}
mtgb::MeshRenderer::MeshRenderer()
	: frame_{ 0 }
	, meshFileName{ "" }
	, meshHandle{ INVALID_HANDLE }
	, layer{ AllLayer() }
	, shaderType{ ShaderType::FbxParts }
{
	meshFileName.resize(BUFFER_SIZE);
}

mtgb::MeshRenderer::MeshRenderer(EntityId _entityId)
	: IComponent(_entityId)
	, meshFileName{ "" }
	, meshHandle{ INVALID_HANDLE }
	, layer{ AllLayer()}
	, shaderType{ ShaderType::FbxParts }
	, frame_{ 0 }
{
	meshFileName.resize(BUFFER_SIZE);
}

void mtgb::MeshRenderer::OnChangeMeshFileName()
{
	meshHandle = Fbx::Load(meshFileName);
}

void mtgb::MeshRenderer::Render() const
{
	if (shaderType == ShaderType::Sea)
	{
		Draw::SeaUVScroll(Transform::Get(GetEntityId()));
	}
	else
	{
		Draw::FBXModel(meshHandle, Transform::Get(GetEntityId()), frame_, shaderType);
	}
}

void mtgb::MeshRenderer::SetFrame(int _frame)
{
	frame_ = _frame;
}

void mtgb::MeshRenderer::OnPostRestore()
{
	if (meshFileName.empty() == false)
	{
		meshHandle = Fbx::Load(meshFileName);
	}
}
