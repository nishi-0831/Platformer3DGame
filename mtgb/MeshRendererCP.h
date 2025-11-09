#pragma once
#include "ComponentPool.h"
#include "MeshRenderer.h"
#include "GameObjectLayer.h"
#include "IRenderable.h"
namespace mtgb
{
    class MeshRenderer;
    /// <summary>
    /// MeshRendererコンポーネントシステム
    /// </summary>
    class MeshRendererCP : public IRenderableCP,public ComponentPool<MeshRenderer,MeshRendererCP>
    {
    public:
        void Initialize();
        void Update() override;
        void RenderAll() const override;
        void RenderLayer(GameObjectLayerFlag _layerFlag)const override;
    };
}