#pragma once
#include "StatefulComponent.h"
#include "IComponentMemento.h"
#include "RectF.h"
#include "UIDrawCommand.h"
#include "TextAlignment.h"
#include "TextRendererCP.h"
#include "IRenderable.h"
namespace mtgb
{
	struct TextRendererData
	{
		std::string text;
		RectF rect;
		int fontSize;
		UIParams params;
		TextAlignment alignment;
		GameObjectLayerFlag layer;
	};

	class TextRendererCP;
	class TextRenderer :public IRenderable , public StatefulComponent<TextRenderer, TextRendererCP,TextRendererData>
	{
	public:
		using StatefulComponent<TextRenderer, TextRendererCP, TextRendererData>::StatefulComponent;
		friend TextRenderer;

		void Render() const override;

		bool CanRender() const override;

		GameObjectLayerFlag GetLayer() const override { return layer; }

	};
}