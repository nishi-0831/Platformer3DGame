#pragma once
#include "IComponent.h"
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
    class MeshRenderer : public IComponent<mtgb::MeshRendererCP, MeshRenderer>
    {
    public:
        friend MeshRendererCP;
        using IComponent<mtgb::MeshRendererCP, MeshRenderer>::IComponent;

        MeshRenderer();
        /// <summary>
        /// メッシュハンドルを設定
        /// </summary>
        void SetMesh(FBXModelHandle meshHandle) { meshHandle_ = meshHandle; }


        /// <summary>
        /// メッシュハンドルを取得
        /// </summary>
        FBXModelHandle GetMesh() const { return meshHandle_; }

        /// <summary>
        /// 使用するシェーダーの種類を返す
        /// </summary>
        /// <returns></returns>
        ShaderType GetShaderType() const { return shaderType_; }
        /// <summary>
        /// 描画可能かチェック
        /// </summary>
        bool CanRender() const { return meshHandle_ != INVALID_HANDLE; }

        GameObjectLayerFlag GetLayer() const { return layer_; }

    private:
        FBXModelHandle meshHandle_ = INVALID_HANDLE;
        GameObjectLayerFlag layer_{AllLayer()};
        ShaderType shaderType_ = ShaderType::FbxParts;
    };
}