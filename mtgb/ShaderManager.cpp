#include "ShaderManager.h"

#include "Box3DShader.h"
#include "MeshShader.h"
#include "SkinnedMeshShader.h"
#include "Debug3DShader.h"
#include "UVScrollShader.h"
#include "OutlineShader.h"
void mtgb::ShaderManager::Initialize()
{
	shaders_.fill(nullptr);

	shaders_[static_cast<int8_t>(ShaderType::BOX3_D)] = new Box3DShader();
	shaders_[static_cast<int8_t>(ShaderType::BOX3_D)]->Initialize(DirectX11Draw::pDevice_.Get());

	shaders_[static_cast<int8_t>(ShaderType::FBX_PARTS)] = new MeshShader();
	shaders_[static_cast<int8_t>(ShaderType::FBX_PARTS)]->Initialize(DirectX11Draw::pDevice_.Get());

	shaders_[static_cast<int8_t>(ShaderType::FBX_PARTS_SKIN)] = new SkinnedMeshShader();
	shaders_[static_cast<int8_t>(ShaderType::FBX_PARTS_SKIN)]->Initialize(DirectX11Draw::pDevice_.Get());

	shaders_[static_cast<int8_t>(ShaderType::DEBUG3_D)] = new Debug3DShader();
	shaders_[static_cast<int8_t>(ShaderType::DEBUG3_D)]->Initialize(DirectX11Draw::pDevice_.Get());

	shaders_[static_cast<int8_t>(ShaderType::SEA)] = new UVScrollShader();
	shaders_[static_cast<int8_t>(ShaderType::SEA)]->Initialize(DirectX11Draw::pDevice_.Get());

	shaders_[static_cast<int8_t>(ShaderType::OUTLINE)] = new OutlineShader();
	shaders_[static_cast<int8_t>(ShaderType::OUTLINE)]->Initialize(DirectX11Draw::pDevice_.Get());
}

void mtgb::ShaderManager::Update() {}

void mtgb::ShaderManager::Release()
{
	for (auto shader : shaders_)
	{
		if (shader != nullptr)
		{
			shader->Release();
			delete shader;
		}
	}
	shaders_.fill(nullptr);
}

IShader& mtgb::ShaderManager::GetShader(ShaderType _shaderType)
{
	IShader* shader = shaders_[static_cast<int8_t>(_shaderType)];
	if (shader == nullptr)
	{
		assert(false);
	}
	return *shader;
}
