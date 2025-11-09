#pragma once
#include "GameObjectLayer.h"
namespace mtgb
{
	class IRenderableCP
	{
	public:
		virtual ~IRenderableCP() = default;

		/// <summary>
		/// ‘S‚Ä•`‰æ
		/// </summary>
		virtual void RenderAll() const = 0;
		/// <summary>
		/// “Á’è‚ÌƒŒƒCƒ„[‚ğ•`‰æ
		/// </summary>
		virtual void RenderLayer(GameObjectLayerFlag _layerFlag) const = 0;
	};

	class IRenderable
	{
	public:
		virtual ~IRenderable() = default;
		virtual void Render() const = 0 ;
		virtual bool CanRender() const = 0;
		virtual GameObjectLayerFlag GetLayer() const = 0;
	};
}