#pragma once
#include "ReflectionMacro.h"
#include "TextRenderer.generated.h"
#include "IComponent.h"
#include "ISerializableObject.h"
#include "IComponentMemento.h"

#include "RectF.h"
#include "UIDrawCommand.h"
#include "TextAlignment.h"
#include "TextRendererCP.h"
#include "IRenderable.h"
namespace mtgb
{
	class TextRendererCP;

	MT_COMPONENT()
	class TextRenderer : public IRenderable, public IComponent<TextRendererCP, TextRenderer>, public ISerializableObject
	{
	  public:
		MT_GENERATED_BODY()
		friend TextRendererCP;

		TextRenderer();
		TextRenderer(EntityId _entityId);
		TextRenderer& operator=(const TextRenderer& _other);
		void Render() const override;

		bool CanRender() const override;

		GameObjectLayerFlag GetLayer() const override
		{
			return params.layerFlag;
		}

		TextAlignment alignment;
		std::string text;
		RectF rect;
		int fontSize;
		UIParams params;
	};
} // namespace mtgb