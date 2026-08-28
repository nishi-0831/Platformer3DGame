#include "Components/RigidBody/RigidBodyCP.h"
#include "Components/Collider/ColliderCP.h"
mtgb::RigidBodyCP::RigidBodyCP() {}

mtgb::RigidBodyCP::~RigidBodyCP() {}

void mtgb::RigidBodyCP::Update()
{
	if (Game::IsEditMode())
		return;
	for (size_t i = 0; i < poolId_.size(); i++)
	{
		if (poolId_[i] == INVALID_ENTITY)
			continue;

		std::vector<Collider*> colliders {};
		Game::System<ColliderCP>().TryGet(&colliders, poolId_[i]);
		RigidBody& rigidBody = pool_[i];
		rigidBody.UpdateVelocity();
		for (auto& collider : colliders)
		{
			if (rigidBody.onHit_)
			{
				collider->ForEachCollisionEnter(rigidBody.onHit_);
			}
			if (rigidBody.onStay_)
			{
				collider->ForEachCollisionStay(rigidBody.onStay_);
			}
			if (rigidBody.onExit_)
			{
				collider->ForEachCollisionExit(rigidBody.onExit_);
			}
		}
	}
}
