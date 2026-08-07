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

	class [[MT_COMPONENT()]] ImageRenderer : public IUIRenderable,
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
		void Initialize() override;
		void Reset() override;
		GameObjectLayerFlag GetLayer() const override;
		UIParams GetUIParams() const override;
		ImageHandle handle_;
		[[MT_PROPERTY()]]
		std::string imageFileName_;
		[[MT_PROPERTY()]]
		RectF drawRect_;
		RectF cutRect_;
		float rotationZ_;
		[[MT_PROPERTY()]]
		UIParams uiParams_;
		[[MT_PROPERTY()]]
		GameObjectLayerFlag layer_;
		Color color_;

	  protected:
		void OnPostRestore() override;
	};
} // namespace mtgb