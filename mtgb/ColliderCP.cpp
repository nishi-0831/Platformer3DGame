#include "ColliderCP.h"
#include "Transform.h"
#include "Model/Fbx.h"

#include "GameObject.h"
mtgb::ColliderCP::ColliderCP() {}

mtgb::ColliderCP::~ColliderCP() {}

void mtgb::ColliderCP::Start()
{
	Collider::hSphereModel_ = Fbx::Load("Model/SphereCollider.fbx");
	massert(Collider::hSphereModel_ >= 0 && "SphereColliderモデルの読み込みに失敗 @ColliderCP::Start");

	Collider::hBoxModel_ = Fbx::Load("Model/BoxCollider.fbx");
	massert(Collider::hBoxModel_ >= 0 && "BoxColliderモデルの読み込みに失敗 @ColliderCP::Start");
}

void mtgb::ColliderCP::Update()
{
	for (size_t i = 0; i < poolId_.size(); i++)
	{
		if (poolId_[i] == INVALID_ENTITY)
		{
			continue;
		}

		Collider& collider		  = pool_[i];
		collider.onCollidersPrev_ = collider.onColliders_;
		collider.onColliders_.clear();
		pool_[i].UpdateBoundingData();
	}

	if (Game::IsEditMode())
		return;

	for (size_t i = 0; i < poolId_.size(); i++)
	{
		if (poolId_[i] == INVALID_ENTITY)
		{
			continue;
		}

		for (size_t j = i + 1; j < poolId_.size(); j++)
		{
			if (poolId_[j] == INVALID_ENTITY)
			{
				continue;
			}

			Collider& colliderA = pool_[i];
			Collider& colliderB = pool_[j];
			if (colliderA.IsHit(colliderB))
			{
				colliderA.onColliders_.insert(&colliderB);
				colliderB.onColliders_.insert(&colliderA);

				colliderA.HandleCollision(colliderB);
				colliderB.HandleCollision(colliderA);
			}
		}
	}
}

void mtgb::ColliderCP::Draw()
{
	for (size_t i = 0; i < poolId_.size(); i++)
	{
		if (poolId_[i] != INVALID_ENTITY)
		{
			pool_[i].Draw();
		}
	}
}
mtgb::EntityId mtgb::ColliderCP::RayCastHitAll(
	const Vector3& _origin,
	const Vector3& _dir,
	float _maxDistance,
	Intersection::RaycastInfo* _info,
	ColliderTag _tag,
	EntityId _ignoreEntityId
)
{
	EntityId nearestEntity = INVALID_ENTITY;
	float nearest		   = _maxDistance;
	Intersection::RaycastInfo nearestInfo;
	for (size_t i = 0; i < poolId_.size(); i++)
	{
		EntityId id = poolId_[i];
		if (id == INVALID_ENTITY)
		{
			continue;
		}
		if (id == _ignoreEntityId)
		{
			continue;
		}

		ColliderTag tag = Get(id).colliderTag_;
		if (tag != _tag && _tag != ColliderTag::GAME_OBJECT)
		{
			continue;
		}
		Intersection::RaycastInfo info;
		if (RayCastHit(_origin, _dir, _maxDistance, &info, id))
		{
			if (info.distance < nearest)
			{
				nearest		  = info.distance;
				nearestEntity = id;
				nearestInfo	  = info;
			}
		}
	}
	*_info = nearestInfo;
	return nearestEntity;
}

bool mtgb::ColliderCP::RayCastHit(
	const Vector3& _origin,
	const Vector3& _dir,
	float _maxDistance,
	mtgb::Intersection::RaycastInfo* _info,
	EntityId _entityId
)
{
	std::vector<Collider*> colliders {};
	if (!TryGet(&colliders, _entityId))
		return false;

	// WARNING: コライダーが一つしかないと断定している。複数になる場合は追加の処理が必要
	if (colliders.empty() == false)
	{
		return colliders.back()->IsHit(_origin, _dir, _maxDistance, _info);
	}
	return false;
}

void mtgb::ColliderCP::IsHitAll(const Vector3& _center, float _radius, std::vector<EntityId>* _entityIds)
{
	_entityIds->clear();

	for (const auto& collider : pool_)
	{
		if (collider.IsHit(_center, _radius))
		{
			_entityIds->push_back(collider.GetEntityId());
		}
	}
}