#include "RigidBodyCP.h"
#include "Transform.h"
#include "GameTime.h"
#include "ColliderCP.h"


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
		{
			std::vector<Collider*> colliders{};
			Game::System<ColliderCP>().TryGet(&colliders, poolId_[i]);
			RigidBody& rigidBody = pool_[i];
			rigidBody.UpdateVelocity();
			for (auto& collider : colliders)
			{
				for (auto& hitCollider : collider->onColliders_)
				{
					rigidBody.onHit_(hitCollider->GetEntityId());
				}
			}
		}
	}
}


