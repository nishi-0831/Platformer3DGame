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
    /// ���b�V���ƃ}�e���A�����Ǘ�����`��R���|�[�l���g
    /// </summary>
    class MeshRenderer : public MeshRendererData, public StatefulComponent<MeshRenderer, MeshRendererCP, MeshRendererData, ComponentMemento<MeshRenderer, MeshRendererData>>
    {
    public:
        using StatefulComponent<MeshRenderer, MeshRendererCP, MeshRendererData, ComponentMemento<MeshRenderer, MeshRendererData>>::StatefulComponent;
        friend MeshRendererCP;

        MeshRenderer();
        /// <summary>
        /// ���b�V���n���h����ݒ�
        /// </summary>
        void SetMesh(FBXModelHandle meshHandle) { meshHandle = meshHandle; }


        /// <summary>
        /// ���b�V���n���h�����擾
        /// </summary>
        FBXModelHandle GetMesh() const { return meshHandle; }

        /// <summary>
        /// �g�p����V�F�[�_�[�̎�ނ�Ԃ�
        /// </summary>
        /// <returns></returns>
        ShaderType GetShaderType() const { return shaderType; }
        /// <summary>
        /// �`��\���`�F�b�N
        /// </summary>
        bool CanRender() const { return meshHandle != INVALID_HANDLE; }

        GameObjectLayerFlag GetLayer() const { return layer; }

    private:
    };

    using MeshRendererMemento = ComponentMemento<MeshRenderer, MeshRendererData>;
}