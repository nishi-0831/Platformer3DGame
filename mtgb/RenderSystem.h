#pragma once
#include "ISystem.h"
#include "GameScene.h"
#include "GameObjectLayer.h"
namespace mtgb
{
	/// <summary>
	/// ウィンドウを描画する処理を呼びだすクラス
	/// </summary>
	class RenderSystem : public ISystem
	{
	  public:
		void Initialize() override;
		void Update() override;
		/// <summary>
		/// ウィンドウを描画。ゲーム画面やImGuiウィンドウすべて描画する
		/// </summary>
		/// <param name="_scene"></param>
		void Render(GameScene& _scene);

	  private:
		/// <summary>
		/// ゲーム画面を描画
		/// </summary>
		/// <param name="_scene"></param>
		void RenderDirectXWindows(GameScene& _scene);
		/// <summary>
		/// ImGuiのウィンドウを描画
		/// </summary>
		/// <param name="_scene"></param>
		void RenderImGuiWindows(GameScene& _scene);
		/// <summary>
		/// シーン内のゲームオブジェクトを描画
		/// </summary>
		/// <param name="_scene"></param>
		/// <param name="_layer"></param>
		void DrawGameObjects(GameScene& _scene, GameObjectLayerFlag _layer);
	};
} // namespace mtgb
