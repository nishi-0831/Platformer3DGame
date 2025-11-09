#pragma once
#include "ComponentPool.h"
#include "TextRenderer.h"
#include "IRenderable.h"

namespace mtgb
{
	class TextRenderer;

	class TextRendererCP : public IRenderableCP, public ComponentPool<TextRenderer, TextRendererCP>
	{
		void Update() override;
		void RenderAll() const override;
		void RenderLayer(GameObjectLayerFlag _layerFlag) const override;
	};
}