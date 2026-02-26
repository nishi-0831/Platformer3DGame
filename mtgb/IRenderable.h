#pragma once
#include "GameObjectLayer.h"
namespace mtgb
{
	/// <summary>
	/// 謠冗判蜃ｦ逅・未騾｣縺ｮ繧､繝ｳ繧ｿ繝ｼ繝輔ぉ繝ｼ繧ｹ縺ｮ繧ｳ繝ｳ繝昴・繝阪Φ繝医・繝ｼ繝ｫ
	/// </summary>
	class IRenderableCP
	{
	  public:
		virtual ~IRenderableCP() = default;

		/// <summary>
		/// 蜈ｨ縺ｦ謠冗判
		/// </summary>
		virtual void RenderAll() const = 0;
		/// <summary>
		/// 迚ｹ螳壹・繝ｬ繧､繝､繝ｼ繧呈緒逕ｻ
		/// </summary>
		virtual void RenderLayer(GameObjectLayerFlag _layerFlag) const = 0;
	};

	/// <summary>
	/// 謠冗判蜃ｦ逅・未騾｣縺ｮ繧､繝ｳ繧ｿ繝ｼ繝輔ぉ繝ｼ繧ｹ
	/// </summary>
	class IRenderable
	{
	  public:
		virtual ~IRenderable() = default;
		/// <summary>
		/// 謠冗判繧定｡後≧
		/// </summary>
		virtual void Render() const = 0;
		/// <summary>
		/// 謠冗判縺悟庄閭ｽ縺句凄縺・
		/// </summary>
		/// <returns> 蜿ｯ閭ｽ縺ｪ繧液rue</returns>
		virtual bool CanRender() const = 0;
		/// <summary>
		/// 繝ｬ繧､繝､繝ｼ繧定ｿ斐☆
		/// </summary>
		/// <returns></returns>
		virtual GameObjectLayerFlag GetLayer() const = 0;
		// 描画を行うか否か
		bool enabled_;
	};
} // namespace mtgb