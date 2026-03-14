#include "stdafx.h"
#include "RespawnManager.h"
#include "GameEvents.h"
#include "ActorManager.h"
#include <format>
RespawnManager::RespawnManager()
	: GameObject()
	, ImGuiShowable(GetEntityId())
	, currentRespawnPointPriority_ { 0 }
	, playerEntityId_ { INVALID_ENTITY }
	, currentRespawnPointEntityId_ { INVALID_ENTITY }
{
	Game::System<EventManager>().GetEvent<PlayerCollideRespawnPointEvent>().Subscribe(
		[this](const PlayerCollideRespawnPointEvent& _event)
		{
			if (currentRespawnPointPriority_ <= _event.priority)
			{
				currentRespawnPointPriority_ = _event.priority;
				currentRespawnPointEntityId_ = _event.respawnPointEntityId;
			}
			playerEntityId_ = _event.playerEntityId;
		}
	);

	// 落下イベントを購読
	Game::System<EventManager>().GetEvent<PlayerFellOutEvent>().Subscribe(
		[this](const PlayerFellOutEvent& _event)
		{
			if (currentRespawnPointEntityId_ == INVALID_ENTITY)
			{
			}
			else
			{
				Transform* pPlayerTransform		  = &Transform::Get(_event.playerEntityId);
				Transform* pRespawnPointTransform = &Transform::Get(currentRespawnPointEntityId_);
				pPlayerTransform->position		  = pRespawnPointTransform->position;
			}
		},
		EventScope::SCENE
	);
}

RespawnManager::~RespawnManager() {}

void RespawnManager::Update() {}

void RespawnManager::ShowImGui()
{
	ImGui::Text("RespawnPointEntityId:%d", currentRespawnPointEntityId_);
	ImGui::Text("PlayerEntityId:%d", playerEntityId_);
}

unsigned int RespawnPoint::generateCounter_ { 0 };

RespawnPoint::RespawnPoint()
	: GameObject()
	, ImGuiShowable(GetEntityId())
	, priority_ { 0 }
	, pTransform_ { Component<Transform>() }
	, pCollider_ { Component<Collider>() }
	, pRigidBody_ { Component<RigidBody>() }
{
	std::string typeName = Game::System<GameObjectTypeRegistry>().GetNameFromType(typeid(RespawnPoint));
	name_				 = std::format("{} ({})", typeName, generateCounter_++);
	displayName_		 = name_;

	pCollider_->isTrigger_	  = true;
	pCollider_->colliderType_ = ColliderType::TYPE_AABB;
	pRigidBody_->isKinematic_ = true;

	pRigidBody_->OnCollisionEnter(
		[this](EntityId _entityId)
		{
			OnCollisionEnter(_entityId);
		}
	);
}

RespawnPoint::~RespawnPoint() {}

void RespawnPoint::Update() {}

void RespawnPoint::ShowImGui()
{
	MTImGui::Instance().ShowComponents(GetEntityId());
	ImGui::Text("EntityId:%d", Entity::entityId_);
}

void RespawnPoint::OnCollisionEnter(EntityId _entityId)
{
	GameObject* pGameObj = Game::System<SceneSystem>().GetActiveScene()->GetGameObject(_entityId);
	if (pGameObj == nullptr)
		return;
	if (pGameObj->GetTag() != GameObjectTag::PLAYER)
		return;

	PlayerCollideRespawnPointEvent event { .playerEntityId		 = _entityId,
										   .respawnPointEntityId = GetEntityId(),
										   .priority			 = priority_ };

	Game::System<EventManager>().GetEvent<PlayerCollideRespawnPointEvent>().Invoke(event);
}
