#pragma once
#include "Core/Component/ComponentPool.h"
#include "Components/TextRenderer/TextRenderer.h"
#include "Graphics/IRenderable.h"

namespace mtgb
{
	class TextRenderer;

	class TextRendererCP : public IRenderableCP, public ComponentPool<TextRenderer, TextRendererCP>
	{
		void Update() override;
		void RenderAll() const override;
		void RenderLayer(GameObjectLayerFlag _layerFlag) const override;
	};
} // namespace mtgb