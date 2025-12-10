#pragma once
#include "ReflectionMacro.h"
#include "MeshRenderer.generated.h"
#include "IComponent.h"
#include "ISerializableObject.h"
#include "IComponentMemento.h"
#include "MeshRendererState.h"
#include "MeshRendererCP.h"
#include "Handlers.h"
#include "GameObjectLayer.h"
#include "ShaderType.h"
#include "IRenderable.h"
#include "cmtgb.h"
namespace mtgb
{
    
    class MeshRendererCP;
    MT_COMPONENT()
    /// <summary>
    /// メッシュとマテリアルを管理する描画コンポーネント
    /// </summary>
    class MeshRenderer :public IRenderable ,public IComponent<MeshRendererCP,MeshRenderer> , public ISerializableObject
    {
    public:
        MT_GENERATED_BODY()
        friend MeshRendererCP;

        MeshRenderer();
        MeshRenderer(EntityId _entityId);
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
        //bool CanRender() const { return meshHandle != INVALID_HANDLE; }

        GameObjectLayerFlag GetLayer() const override { return layer; }
        void OnChangeMeshFileName();
        void Render() const override;
        bool CanRender() const override { return meshHandle != INVALID_HANDLE; };

        MT_PROPERTY()
        std::string meshFileName;
        MT_PROPERTY()
        FBXModelHandle meshHandle;
        MT_PROPERTY()
        GameObjectLayerFlag layer;
        MT_PROPERTY()
        ShaderType shaderType;
    protected:
        void OnPostRestore() override;
    private:
    };

    using MeshRendererMemento = ComponentMemento<MeshRenderer, MeshRendererState>;
}