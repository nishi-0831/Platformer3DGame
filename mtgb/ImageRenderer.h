#pragma once
#include "ReflectionMacro.h"
#include "ImageRenderer.generated.h"
#include "IComponent.h"
#include "ISerializableObject.h"
#include "IComponentMemento.h"
#include "ImageRendererCP.h"
#include "RectF.h"
#include "UIDrawCommand.h"
#include "IRenderable.h"

namespace mtgb
{
	class ImageRendererCP;

	MT_COMPONENT()
		class ImageRenderer : public IRenderable, public IComponent<ImageRendererCP, ImageRenderer>, public ISerializableObject
	{
	public:
		MT_GENERATED_BODY()
			friend ImageRendererCP;

		ImageRenderer();
		ImageRenderer(EntityId _entityId);

		// IRenderable ÇâÓÇµÇƒåpè≥Ç≥ÇÍÇ‹ÇµÇΩ
		void Render() const override;

		bool CanRender() const override;

		GameObjectLayerFlag GetLayer() const override;

		ImageHandle handle_;
		RectF drawRect_;
		RectF cutRect_;
		float rotationZ_;
		UIParams uiParams_;
		GameObjectLayerFlag layer_;
	};
}