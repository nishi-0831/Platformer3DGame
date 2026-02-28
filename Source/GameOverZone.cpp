#include "stdafx.h"
#include "GameOverZone.h"
#include "ResultScene.h"
#include "GameEvents.h"
#include "ActorManager.h"
unsigned int GameOverZone::generateCounter_ { 0 };

GameOverZone::GameOverZone()
	: GameObject()
	, pTransform_ { Component<Transform>() }
	, pCollider_ { Component<Collider>() }
	, pRigidBody_ { Component<RigidBody>() }
	, takeDamageAmoundOnPlayerFellout_ { 1 }
{
	pTransform_->position	  = Vector3(0, -3, 0);
	pCollider_->colliderType_ = ColliderType::TYPE_AABB;
	pCollider_->isStatic_	  = false;
	pCollider_->SetExtents(pTransform_->scale * 0.5f);
	std::string typeName = Game::System<GameObjectTypeRegistry>().GetNameFromType(typeid(GameOverZone));
	name_				 = std::format("{} ({})", typeName, generateCounter_++);
}

GameOverZone::~GameOverZone() {}

void GameOverZone::Update() {}

void GameOverZone::Start()
{
	pTransform_ = Component<Transform>();

	pRigidBody_->OnCollisionEnter(
		[this](EntityId _entityId)
		{
			IActor* pActor	  = Game::System<ActorManager>().GetActor(_entityId);
			GameObjectTag tag = FindGameObject(_entityId)->GetTag();
			if (tag == GameObjectTag::PLAYER)
			{
				// 落下イベント通知
				PlayerFellOutEvent event { .playerEntityId = _entityId };
				Game::System<EventManager>().GetEvent<PlayerFellOutEvent>().Invoke(event);
				if (pActor == nullptr)
					return;
				pActor->TakeDamage(takeDamageAmoundOnPlayerFellout_);
			}
			else
			{
				if (pActor == nullptr)
					return;
				// プレイヤー以外は倒す
				pActor->TakeDamage(INT_MAX);
			}
		}
	);
}

void GameOverZone::Draw() const {}
