#pragma once
#include "GameObjectLayer.h"
namespace mtgb
{
	/// <summary>
	/// 描画処理関連のインターフェースのコンポーネントプール
	/// </summary>
	class IRenderableCP
	{
	public:
		virtual ~IRenderableCP() = default;

		/// <summary>
		/// 全て描画
		/// </summary>
		virtual void RenderAll() const = 0;
		/// <summary>
		/// 特定のレイヤーを描画
		/// </summary>
		virtual void RenderLayer(GameObjectLayerFlag _layerFlag) const = 0;
	};

	/// <summary>
	/// 描画処理関連のインターフェース
	/// </summary>
	class IRenderable
	{
	public:
		virtual ~IRenderable() = default;
		/// <summary>
		/// 描画を行う
		/// </summary>
		virtual void Render() const = 0 ;
		/// <summary>
		/// 描画が可能か否か
		/// </summary>
		/// <returns> 可能ならtrue</returns>
		virtual bool CanRender() const = 0;
		/// <summary>
		/// レイヤーを返す
		/// </summary>
		/// <returns></returns>
		virtual GameObjectLayerFlag GetLayer() const = 0;
	};
}