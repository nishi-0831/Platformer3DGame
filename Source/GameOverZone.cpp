#include "stdafx.h"
#include "GameOverZone.h"
#include "ResultScene.h"
unsigned int GameOverZone::generateCounter_{ 0 };

GameOverZone::GameOverZone()
    : GameObject()
    , pTransform_{ Component<Transform>() }
    , pCollider_{ Component<Collider>() }
{
    pTransform_->position = Vector3(0, -3, 0);
    pCollider_->colliderType_ = ColliderType::TYPE_AABB;
    pCollider_->isStatic_ = false;
    pCollider_->SetExtents(pTransform_->scale * 0.5f);
    std::string typeName = Game::System<GameObjectTypeRegistry>().GetNameFromType(typeid(GameOverZone));
    name_ = std::format("{} ({})", typeName, generateCounter_++);
}

GameOverZone::~GameOverZone()
{
}

void GameOverZone::Update()
{
}

void GameOverZone::Start()
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

void GameOverZone::Draw() const
{
}

