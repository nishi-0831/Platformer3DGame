#pragma once
#include "Editor/ReflectionMacro.h"
#include "IComponent.h"
#include "ISerializableObject.h"
#include "IComponentMemento.h"

#include "RectF.h"
#include "UIParams.h"
#include "TextAlignment.h"
#include "TextRendererCP.h"
#include "IRenderable.h"
#include "TextRenderer.generated.h"
namespace mtgb
{
	class TextRendererCP;

	class [[MT_COMPONENT()]] TextRenderer : public IUIRenderable,
											public IComponent<TextRendererCP, TextRenderer>,
											public ISerializableObject
	{
	  public:
		MT_GENERATED_BODY()
		friend TextRendererCP;

		TextRenderer();
		TextRenderer(EntityId _entityId);
		void Render() const override;
		void Initialize() override;
		void Reset() override;
		bool CanRender() const override;
		UIParams GetUIParams() const override;
		GameObjectLayerFlag GetLayer() const override
		{
			return params_.layerFlag;
		}
		[[MT_PROPERTY()]]
		TextAlignment alignment;
		[[MT_PROPERTY()]]
		std::string text_;
		[[MT_PROPERTY()]]
		RectF rect_;
		[[MT_PROPERTY()]]
		int fontSize_;
		[[MT_PROPERTY()]]
		UIParams params_;
	};
} // namespace mtgb