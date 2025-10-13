#pragma once
#include "ComponentPool.h"
#include "MeshRenderer.h"
#include "GameObjectLayer.h"
namespace mtgb
{
    /// <summary>
    /// MeshRenderer�R���|�[�l���g�V�X�e��
    /// </summary>
    class MeshRendererCP : public ComponentPool<MeshRenderer>
    {
    public:
        void Initialize() override;
        void Update() override;

        /// <summary>
        /// �S�Ă�MeshRenderer��`��
        /// </summary>
        void RenderAll();

        /// <summary>
        /// ����̃��C���[��MeshRenderer��`��
        /// </summary>
        void RenderLayer(GameObjectLayerFlag layerFlag);
    };
}