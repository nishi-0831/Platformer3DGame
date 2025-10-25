#pragma once
#include "Entity.h"
#include "GameObjectLayer.h"
#include "Handlers.h"
#include "DirectX11Draw.h"
namespace mtgb
{
	/// <summary>
	/// MeshRenderer‚Ìó‘Ô‚Ì‚İ‚ğ•Û‚·‚é\‘¢‘Ì
	/// </summary>
	struct MeshRendererData
	{
		FBXModelHandle meshHandle{ INVALID_ENTITY };
		GameObjectLayerFlag layer{ AllLayer() };
		ShaderType shaderType{ ShaderType::FbxParts };
	};
}
