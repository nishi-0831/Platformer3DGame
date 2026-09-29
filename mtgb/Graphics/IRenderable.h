#pragma once
#include <functional>
#include "Core/GameObject/GameObjectLayer.h"
#include "Graphics/UIParams.h"
namespace mtgb
{
	/// <summary>
	/// 描画処理関連のコンポーネントプールのインターフェース
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
		IRenderable()
			: enabled_ { true } {};
		virtual ~IRenderable() = default;
		/// <summary>
		/// 描画を行う
		/// </summary>
		virtual void Render() const = 0;
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
		/// <summary>
		/// 描画直前にコールバックを呼ぶ
		/// コールバックはSetOnPreRenderCallbackで設定可能
		/// </summary>
		void OnPreRender() const
		{
			if (onPreRenderCallback_)
			{
				onPreRenderCallback_();
			}
		}
		/// <summary>
		/// 描画直前に呼ばれるコールバックを設定
		/// </summary>
		/// <typeparam name="Func">引数なしで呼び出し可能な型</typeparam>
		/// <param name="_callback"></param>
		template <typename Func>
			requires std::is_invocable_v<Func>
		void SetOnPreRenderCallback(Func&& _callback)
		{
			onPreRenderCallback_ = std::forward<Func>(_callback);
		}
		// 描画を行うか否か
		bool enabled_;
		std::function<void(void)> onPreRenderCallback_;
	};

	class IUIRenderable : public IRenderable
	{
	  public:
		virtual UIParams GetUIParams() const = 0;
	};
} // namespace mtgb