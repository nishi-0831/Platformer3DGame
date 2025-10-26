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
    /// ���b�V���ƃ}�e���A�����Ǘ�����`��R���|�[�l���g
    /// </summary>
    class MeshRenderer : public IComponent<mtgb::MeshRendererCP, MeshRenderer>
    {
    public:
        friend MeshRendererCP;
        using IComponent<mtgb::MeshRendererCP, MeshRenderer>::IComponent;

        MeshRenderer();
        /// <summary>
        /// ���b�V���n���h����ݒ�
        /// </summary>
        void SetMesh(FBXModelHandle meshHandle) { meshHandle_ = meshHandle; }


        /// <summary>
        /// ���b�V���n���h�����擾
        /// </summary>
        FBXModelHandle GetMesh() const { return meshHandle_; }

        /// <summary>
        /// �g�p����V�F�[�_�[�̎�ނ�Ԃ�
        /// </summary>
        /// <returns></returns>
        ShaderType GetShaderType() const { return shaderType_; }
        /// <summary>
        /// �`��\���`�F�b�N
        /// </summary>
        bool CanRender() const { return meshHandle_ != INVALID_HANDLE; }

        GameObjectLayerFlag GetLayer() const { return layer_; }

    private:
        FBXModelHandle meshHandle_ = INVALID_HANDLE;
        GameObjectLayerFlag layer_{AllLayer()};
        ShaderType shaderType_ = ShaderType::FbxParts;
    };
}