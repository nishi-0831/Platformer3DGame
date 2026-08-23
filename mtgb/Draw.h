#pragma once

#include "Core/ISystem.h"
#include "ShaderType.h"
#include "IRenderable.h"

namespace mtgb
{
	class Transform;

	class Draw final : public ISystem
	{
		friend class RenderSystem;

	  public:
	  public:
		Draw();
		~Draw();
		void Initialize() override;
		void Update() override;
		/// <summary>
		/// UIを描画する
		/// RegisterUIElementで登録したものを描画
		/// </summary>
		/// <param name="_layer"></param>
		void RenderUI(GameObjectLayerFlag _layer);
		/// <summary>
		/// 画面のサイズに合わせてフォントサイズを計算する
		/// </summary>
		/// <param name="_baseSize"></param>
		/// <returns></returns>
		int CalcScaledFontSize(int _baseSize);
		/// <summary>
		/// UI要素を登録する
		/// </summary>
		/// <param name="_e"></param>
		void RegisterUIElement(IUIRenderable* _e);
		/// <summary>
		/// UI要素を登録解除する
		/// </summary>
		/// <param name="_e"></param>
		void UnregisterUIElement(IUIRenderable* _e);

	  private:
		// UI要素の配列
		std::vector<IUIRenderable*> uiElements_;
		// ダーティフラグ。UI要素の配列が更新された時にオンになる
		bool uiElementDirty_;
	};
} // namespace mtgb
