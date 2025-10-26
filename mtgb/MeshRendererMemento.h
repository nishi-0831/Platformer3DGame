#pragma once
#include "IComponentMemento.h"
#include "MeshRenderer.h"
#include "Entity.h"
#include "Handlers.h"

namespace mtgb
{
	/*class MeshRendererMemento : public IComponentMemento
	{
		friend MeshRenderer;
	public:
		explicit MeshRendererMemento(EntityId _entityId,FBXModelHandle _hMesh,GameObjectLayerFlag _layer,ShaderType _shaderType)
			: entityId_{ _entityId }
			, hMesh_{_hMesh}
			, layer_{_layer}
			, shaderType_{_shaderType}
		{
		}
		
		std::type_index GetComponentType() const override
		{
			return std::type_index(typeid(class MeshRenderer));
		}

		EntityId GetEntityId() const override { return entityId_; }
	private:
		EntityId entityId_;
		FBXModelHandle hMesh_;
		GameObjectLayerFlag layer_;
		ShaderType shaderType_;
	};*/
}