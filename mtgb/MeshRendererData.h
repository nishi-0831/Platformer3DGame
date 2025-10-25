#pragma once
#include "Entity.h"
#include "GameObjectLayer.h"
#include "Handlers.h"
#include "DirectX11Draw.h"
namespace mtgb
{
	/// <summary>
	/// MeshRenderer�̏�Ԃ݂̂�ێ�����\����
	/// </summary>
	struct MeshRendererData
	{
		FBXModelHandle meshHandle{ INVALID_ENTITY };
		GameObjectLayerFlag layer{ AllLayer() };
		ShaderType shaderType{ ShaderType::FbxParts };
	};
}
