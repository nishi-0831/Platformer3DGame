#pragma once
#include "StatefulComponent.h"
#include "IComponentMemento.h"
#include "MeshRendererData.h"
#include "MeshRendererCP.h"
#include "Handlers.h"
#include "GameObjectLayer.h"
#include "ShaderType.h"
namespace mtgb
{
    
    class MeshRendererCP;
    /// <summary>
    /// メッシュとマテリアルを管理する描画コンポーネント
    /// </summary>
    class MeshRenderer : public MeshRendererData, public StatefulComponent<MeshRenderer, MeshRendererCP, MeshRendererData>
    {
    public:
        using StatefulComponent<MeshRenderer, MeshRendererCP, MeshRendererData>::StatefulComponent;
        friend MeshRendererCP;

        MeshRenderer();
        /// <summary>
        /// メッシュハンドルを設定
        /// </summary>
        void SetMesh(FBXModelHandle meshHandle) { meshHandle = meshHandle; }

        /// <summary>
        /// メッシュハンドルを取得
        /// </summary>
        FBXModelHandle GetMesh() const { return meshHandle; }

        /// <summary>
        /// 使用するシェーダーの種類を返す
        /// </summary>
        /// <returns></returns>
        ShaderType GetShaderType() const { return shaderType; }
        /// <summary>
        /// 描画可能かチェック
        /// </summary>
        bool CanRender() const { return meshHandle != INVALID_HANDLE; }

        GameObjectLayerFlag GetLayer() const { return layer; }
    protected:
        void OnPostRestore() override;
    private:
    };

    using MeshRendererMemento = ComponentMemento<MeshRenderer, MeshRendererData>;
}