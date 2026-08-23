#include <fbxsdk.h>
#include "MeshRenderer.h"
#include "Model/Fbx.h"
#include "Draw.h"
#include "Transform.h"
#include "ShadowSettings.h"

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
	, pAnimStack_ { nullptr }
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
	, pAnimStack_ { nullptr }
{
	enabled_ = true;
	meshFileName.resize(BUFFER_SIZE);
}

void mtgb::MeshRenderer::OnChangeMeshFileName()
{
	meshHandle = Fbx::Load(meshFileName);
}

void mtgb::MeshRenderer::Render() const
{
	if (shaderType == ShaderType::BOX3_D)
	{
		Game::System<ShadowSettings>().SetCB();
	}

	if (pAnimStack_ != nullptr)
	{
		pAnimStack_->GetScene()->SetCurrentAnimationStack(pAnimStack_);
	}

	Game::System<Fbx>().Draw(meshHandle, Transform::Get(GetEntityId()), frame_, shaderType);
}

void mtgb::MeshRenderer::SetFrame(int _frame)
{
	frame_ = _frame;
}

void mtgb::MeshRenderer::SetAnimStack(FbxAnimStack* _pAnimStack)
{
	pAnimStack_ = _pAnimStack;
}

void mtgb::MeshRenderer::OnPostRestore()
{
	if (meshFileName.empty() == false)
	{
		meshHandle = Fbx::Load(meshFileName);
	}
}
