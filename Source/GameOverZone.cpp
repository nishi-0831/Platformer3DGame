#include "stdafx.h"
#include "GameOverZone.h"
#include "ResultScene.h"
unsigned int GameOverZone::generateCounter_{ 0 };

GameOverZone::GameOverZone()
    : GameObject()
{
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

std::vector<IComponentMemento*> GameOverZone::GetDefaultMementos(EntityId _entityId) const
{
    std::vector<IComponentMemento*> mementos;

    TransformState transformState
    {
        .position{0,-3,0},
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

    mementos.push_back(new TransformMemento(_entityId, transformState));
    mementos.push_back(new ColliderMemento(_entityId, colliderState));
    return mementos;
}
