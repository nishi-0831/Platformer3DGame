#pragma once
#include "ComponentPool.h"
#include "ImageRenderer.h"
#include "GameObjectLayer.h"
#include "IRenderable.h"
namespace mtgb
{
	class ImageRenderer;
	/// <summary>
	/// ImageRendererコンポーネントシステム
	/// </summary>
	class ImageRendererCP : public IRenderableCP, public ComponentPool<ImageRenderer, ImageRendererCP>
	{
	  public:
		void Initialize();
		void Update() override;
		void RenderAll() const override;
		void RenderLayer(GameObjectLayerFlag _layerFlag) const override;
	};
} // namespace mtgb