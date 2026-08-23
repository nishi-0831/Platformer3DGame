#pragma once
#include "Editor/ReflectionMacro.h"
#include "IComponent.h"
#include "ISerializableObject.h"
#include "IComponentMemento.h"
#include "ImageRendererCP.h"
#include "RectF.h"
#include "UIParams.h"
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
		/// <summary>
		/// 描画範囲(スクリーン座標系)
		/// </summary>
		[[MT_PROPERTY()]]
		RectF drawRect_;
		/// <summary>
		/// 画像から切り取って描画する範囲
		/// </summary>
		RectF cutRect_;
		/// <summary>
		/// 画像の角度
		/// </summary>
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