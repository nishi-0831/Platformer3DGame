#pragma once
#include "ReflectionMacro.h"
#include "IComponent.h"
#include "ISerializableObject.h"
#include "IComponentMemento.h"
#include "ImageRendererCP.h"
#include "RectF.h"
#include "UIDrawCommand.h"
#include "IRenderable.h"
#include "ImageRenderer.generated.h"

namespace mtgb
{
	class ImageRendererCP;

	class [[MT_COMPONENT()]] ImageRenderer : public IRenderable,
											 public IComponent<ImageRendererCP, ImageRenderer>,
											 public ISerializableObject
	{
	  public:
		MT_GENERATED_BODY()
		friend ImageRendererCP;

		ImageRenderer();
		ImageRenderer(EntityId _entityId);

		// IRenderable を介して継承されました
		void Render() const override;

		bool CanRender() const override;

		GameObjectLayerFlag GetLayer() const override;

		ImageHandle handle_;
		std::string imageFileName_;
		RectF drawRect_;
		RectF cutRect_;
		float rotationZ_;
		UIParams uiParams_;
		GameObjectLayerFlag layer_;
		Color color_;
	};
} // namespace mtgb