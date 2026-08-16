#include "ShaderManager.h"

#include "Box3DShader.h"
#include "MeshShader.h"
#include "SkinnedMeshShader.h"
#include "Debug3DShader.h"
#include "UVScrollShader.h"
void mtgb::ShaderManager::Initialize()
{
	shader_.fill(nullptr);
	shader_[static_cast<int8_t>(ShaderType::BOX3_D)] = new Box3DShader();
	shader_[static_cast<int8_t>(ShaderType::BOX3_D)]->Initialize(DirectX11Draw::pDevice_.Get());

	shader_[static_cast<int8_t>(ShaderType::FBX_PARTS)] = new MeshShader();
	shader_[static_cast<int8_t>(ShaderType::FBX_PARTS)]->Initialize(DirectX11Draw::pDevice_.Get());

	shader_[static_cast<int8_t>(ShaderType::FBX_PARTS_SKIN)] = new SkinnedMeshShader();
	shader_[static_cast<int8_t>(ShaderType::FBX_PARTS_SKIN)]->Initialize(DirectX11Draw::pDevice_.Get());

	shader_[static_cast<int8_t>(ShaderType::DEBUG3_D)] = new Debug3DShader();
	shader_[static_cast<int8_t>(ShaderType::DEBUG3_D)]->Initialize(DirectX11Draw::pDevice_.Get());

	shader_[static_cast<int8_t>(ShaderType::SEA)] = new UVScrollShader();
	shader_[static_cast<int8_t>(ShaderType::SEA)]->Initialize(DirectX11Draw::pDevice_.Get());
}

void mtgb::ShaderManager::Update() {}

void mtgb::ShaderManager::Release() {}

IShader& mtgb::ShaderManager::GetShader(ShaderType _shaderType)
{
	IShader* shader = shader_[static_cast<int8_t>(_shaderType)];
	if (shader == nullptr)
	{
		assert(false);
	}
	return *shader;
}
