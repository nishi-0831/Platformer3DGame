#pragma once
#include "ComponentPool.h"
#include "MeshRenderer.h"
#include "GameObjectLayer.h"
namespace mtgb
{
    /// <summary>
    /// MeshRendererコンポーネントシステム
    /// </summary>
    class MeshRendererCP : public ComponentPool<MeshRenderer>
    {
    public:
        void Initialize() override;
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