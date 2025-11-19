#include "RigidBodyCP.h"
#include "Transform.h"
#include "GameTime.h"
#include "ColliderCP.h"
#include <algorithm>

mtgb::RigidBodyCP::RigidBodyCP()
{
}

mtgb::RigidBodyCP::~RigidBodyCP()
{
}

void mtgb::RigidBodyCP::Update()
{
	for (size_t i = 0; i < poolId_.size(); i++)
	{
		if (poolId_[i] != INVALID_ENTITY)
			continue;

		std::vector<Collider*> colliders{};
		Game::System<ColliderCP>().TryGet(&colliders, poolId_[i]);
		RigidBody& rigidBody = pool_[i];
		rigidBody.UpdateVelocity();
		for (auto& collider : colliders)
		{

			for (Collider* collider : collider->onColliders_)
			{
				if (collider->onColldiersPrev_.find(collider) == collider->onColldiersPrev_.end())
				{
					rigidBody.onHit_(collider->GetEntityId());
				}
				else
				{
					rigidBody.onStay_(collider->GetEntityId());
				}
			}

			for (Collider* collider : collider->onColldiersPrev_)
			{
				if (collider->onColliders_.find(collider) == collider->onColliders_.end())
				{
					rigidBody.onExit_(collider->GetEntityId());
				}
			}
		}
	}
}


