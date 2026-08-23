#pragma once
#include "Core/Component/ComponentPool.h"
#include "Components/MeshRenderer/MeshRenderer.h"
#include "Core/GameObject/GameObjectLayer.h"
#include "IRenderable.h"
namespace mtgb
{
	class MeshRenderer;
	/// <summary>
	/// MeshRendererコンポーネントシステム
	/// </summary>
	class MeshRendererCP : public IRenderableCP, public ComponentPool<MeshRenderer, MeshRendererCP>
	{
	  public:
		void Update() override;
		void RenderAll() const override;
		void RenderLayer(GameObjectLayerFlag _layerFlag) const override;
	};
} // namespace mtgb