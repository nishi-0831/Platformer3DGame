#include "RigidBody.h"
#include "Transform.h"
#include <DirectXCollision.h>

mtgb::RigidBody::RigidBody(const EntityId _entityId) :
	StatefulComponent{ RigidBodyData{.isNeedUpdate = false},_entityId },
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

Vector3 mtgb::RigidBody::ColliderSphere(const DirectX::BoundingSphere& _sphere, const DirectX::BoundingBox& _aabb)
{
	return Vector3();
}


