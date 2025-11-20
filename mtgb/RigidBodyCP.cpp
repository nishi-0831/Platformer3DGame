#include "RigidBodyCP.h"
#include "Transform.h"
#include "GameTime.h"
#include "ColliderCP.h"
#include <algorithm>
#include "Debug.h"
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
		if (poolId_[i] == INVALID_ENTITY)
			continue;

		std::vector<Collider*> colliders{};
		Game::System<ColliderCP>().TryGet(&colliders, poolId_[i]);
		RigidBody& rigidBody = pool_[i];
		rigidBody.UpdateVelocity();
		for (auto& collider : colliders)
		{
			for (Collider* onCollider : collider->onColliders_)
			{
				// ˆÈ‘O‚ÍÕ“Ë‚µ‚Ä‚¢‚È‚¢‚©”Û‚©
				if (collider->onColldiersPrev_.find(onCollider) == collider->onColldiersPrev_.end())
				{
					if (rigidBody.onHit_)
					{
						rigidBody.onHit_(onCollider->GetEntityId());
					}
				}
				else
				{
					if (rigidBody.onStay_)
					{
						rigidBody.onStay_(onCollider->GetEntityId());
					}
				}
			}

			for (Collider* onColliderPrev : collider->onColldiersPrev_)
			{
				// Œ»Ý‚ÍÕ“Ë‚µ‚Ä‚¢‚È‚¢‚©”Û‚©
				if (collider->onColliders_.find(onColliderPrev) == collider->onColliders_.end())
				{
					if (rigidBody.onExit_)
					{
						rigidBody.onExit_(onColliderPrev->GetEntityId());
					}

				}
			}
		}
	}
}


