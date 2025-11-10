#include "stdafx.h"
#include "Goal.h"
#include "ResultScene.h"
Goal::Goal()
{
}

Goal::~Goal()
{
}

void Goal::Update()
{
}

void Goal::Start()
{
    pTransform_ = Component<Transform>();
    pRigidBody_ = Component<RigidBody>();

    pRigidBody_->OnCollisionEnter([this](EntityId _entityId)
        {
            GameObjectTag tag = FindGameObject(_entityId)->GetTag();
            if (tag == GameObjectTag::Player)
            {
                Game::System<SceneSystem>().Move<ResultScene>();
            }
        });
}

void Goal::Draw() const
{

}

std::vector<IComponentMemento*> Goal::GetDefaultMementos(EntityId _entityId) const
{
    std::vector<IComponentMemento*> mementos;

    TransformData transformData
    {
        .position{0,0,5},
        .scale{1,1,1}
    };

    ColliderData colliderData
    {
        .colliderType{ColliderType::TYPE_SPHERE},
        .isStatic{false},
        .colliderTag{},
        .center{transformData.position},
        .extents{transformData.scale}
    };

    MeshRendererData meshData
    {
        .meshFileName{"Model/Box3D.fbx"},
        .meshHandle{Fbx::Load(meshData.meshFileName)},
        .layer{AllLayer()},
        .shaderType{ShaderType::FbxParts}
    };

    mementos.push_back(new TransformMemento(_entityId, transformData));
    mementos.push_back(new ColliderMemento(_entityId, colliderData));
    mementos.push_back(new MeshRendererMemento(_entityId, meshData));
    return mementos;
}
