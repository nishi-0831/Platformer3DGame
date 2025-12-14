#include "stdafx.h"
#include "Goal.h"
#include "ResultScene.h"
#include "StageManager.h"
unsigned int Goal::generateCounter_{ 0 };

Goal::Goal()
    :GameObject()
    , pTransform_{ Component<Transform>() }
    , pCollider_{ Component<Collider>() }
    , pRigidBody_{ Component<RigidBody>() }
    , pMeshRenderer_{Component<MeshRenderer>()}
{
    pCollider_->colliderType_ = ColliderType::TYPE_AABB;
    pCollider_->isStatic_ = false;
    pCollider_->SetExtents(pTransform_->scale * 0.5f);
    pMeshRenderer_->meshFileName = "Model/WallBox.fbx";
    pMeshRenderer_->SetMesh(Fbx::Load(pMeshRenderer_->meshFileName));
    pMeshRenderer_->layer = AllLayer();
    pMeshRenderer_->shaderType = ShaderType::FbxParts;

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

