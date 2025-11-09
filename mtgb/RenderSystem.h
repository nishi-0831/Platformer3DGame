#pragma once
#include "ISystem.h"
#include "GameScene.h"
#include "ReleaseUtility.h"
#include"GameObjectLayer.h"
namespace mtgb
{
	class GameObject;

	class RenderSystem : public ISystem
	{
	public:
		void Initialize() override;
		void Update() override;
		void Render(GameScene& _scene);
	private:
		void RenderDirectXWindows(GameScene& _scene);
		void RenderImGuiWindows(GameScene& _scene);
		void DrawGameObjects(GameScene& _scene,GameObjectLayerFlag _layer);
	};
}
