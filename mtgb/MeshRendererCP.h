#pragma once
#include "ComponentPool.h"
#include "MeshRenderer.h"
#include "GameObjectLayer.h"
namespace mtgb
{
    class MeshRenderer;
    /// <summary>
    /// MeshRendererコンポーネントシステム
    /// </summary>
    class MeshRendererCP : public ComponentPool<MeshRenderer,MeshRendererCP>
    {
    public:
        void Initialize();
        void Update() override;

        /// <summary>
        /// 全てのMeshRendererを描画
        /// </summary>
        void RenderAll();

        /// <summary>
        /// 特定のレイヤーのMeshRendererを描画
        /// </summary>
        void RenderLayer(GameObjectLayerFlag layerFlag);
    };
}