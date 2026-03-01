#include "MeshRenderer.h"
#include "Fbx.h"
#include "Draw.h"
#include "Transform.h"
namespace
{
	constexpr size_t BUFFER_SIZE { 256 };
}
mtgb::MeshRenderer::MeshRenderer()
	: meshFileName { "" }
	, meshHandle { INVALID_HANDLE }
	, layer { AllLayer() }
	, shaderType { ShaderType::FBX_PARTS }
	, frame_ { 0 }
{
	enabled_ = true;
	meshFileName.resize(BUFFER_SIZE);
}

mtgb::MeshRenderer::MeshRenderer(EntityId _entityId)
	: IComponent(_entityId)
	, meshFileName { "" }
	, meshHandle { INVALID_HANDLE }
	, layer { AllLayer() }
	, shaderType { ShaderType::FBX_PARTS }
	, frame_ { 0 }
{
	enabled_ = true;
	meshFileName.resize(BUFFER_SIZE);
}

mtgb::MeshRenderer& mtgb::MeshRenderer::operator=(const MeshRenderer& _other)
{
	meshFileName = _other.meshFileName;
	meshHandle	 = _other.meshHandle;
	layer		 = _other.layer;
	shaderType	 = _other.shaderType;
	frame_		 = _other.frame_;

	return *this;
}

void mtgb::MeshRenderer::OnChangeMeshFileName()
{
	meshHandle = Fbx::Load(meshFileName);
}

void mtgb::MeshRenderer::Render() const
{
	if (shaderType == ShaderType::SEA)
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
