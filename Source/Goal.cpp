#include "stdafx.h"
#include "Goal.h"
#include "ResultScene.h"
#include "StageManager.h"
unsigned int Goal::generateCounter_{ 0 };

Goal::Goal()
    :GameObject()
{
    std::string typeName = Game::System<GameObjectTypeRegistry>().GetNameFromType(typeid(Goal));
    name_ = std::format("{} ({})", typeName, generateCounter_++);
    displayName_ = name_;

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
                Game::System<StageManger>().ClearCurrentStage();
                Game::System<SceneSystem>().Move<ResultScene>();
            }
        });
}

void Goal::Draw() const
{

}

void Goal::ShowImGui()
{
    MTImGui::Instance().ShowComponents(Entity::entityId_);
    ImGui::Text("EntityId:%d", Entity::entityId_);
}

std::vector<IComponentMemento*> Goal::GetDefaultMementos(EntityId _entityId) const
{
    std::vector<IComponentMemento*> mementos;

    TransformState transformState
    {
        .position{0,1,5},
        .scale{1,1,1}
    };

    ColliderState colliderState
    {
        .colliderType{ColliderType::TYPE_AABB},
        .isStatic{false},
        .colliderTag{},
        .center{transformState.position},
        .extents{transformState.scale * 0.5f}
    };

    MeshRendererState meshData
    {
        .meshFileName{"Model/Box.fbx"},
        .meshHandle{Fbx::Load(meshData.meshFileName)},
        .layer{AllLayer()},
        .shaderType{ShaderType::FbxParts}
    };

    mementos.push_back(new TransformMemento(_entityId, transformState));
    mementos.push_back(new ColliderMemento(_entityId, colliderState));
    mementos.push_back(new MeshRendererMemento(_entityId, meshData));
    return mementos;
}
