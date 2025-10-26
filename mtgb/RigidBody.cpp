#include "RigidBody.h"
#include "RigidBodyMemento.h"
#include "Transform.h"

mtgb::RigidBody::RigidBody(const EntityId _entityId) :
	StatefulComponent{ _entityId },
	RigidBodyData{.isNeedUpdate = false},
	pTransform_{ &Transform::Get(_entityId) },
	onHit_{ [](const EntityId) {} }
{
}

mtgb::RigidBody::~RigidBody()
{
}

void mtgb::RigidBody::OnCollisionEnter(const std::function<void(const EntityId)>& _onHit)
{
	isNeedUpdate = true;
	onHit_ = _onHit;
}


