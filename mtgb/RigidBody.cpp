#include "RigidBody.h"
#include "Transform.h"
#include <DirectXCollision.h>
#include "BoxPlane.h"
#include <tuple>
#include <vector>
#include "GameTime.h"
namespace
{
	float GRAVITY{ -20.0f };
}
mtgb::RigidBody::RigidBody(const EntityId _entityId) :
	StatefulComponent{ RigidBodyState{.isNeedUpdate = false,.useGravity = false},_entityId },
	pTransform_{ &Transform::Get(_entityId) },
	onHit_{ [](const EntityId) {} }
{
}

mtgb::RigidBody::~RigidBody()
{
}

void mtgb::RigidBody::UpdateVelocity()
{
	if (useGravity)
	{
		velocity += Vector3::Up() * GRAVITY * Time::DeltaTimeF();
	}
	pTransform_->position += velocity * Time::DeltaTimeF();
	if (velocity.y > 0.0f)
	{
		isGround = false;
	}
}

void mtgb::RigidBody::OnGround()
{
	isGround = true;
	velocity.y = 0.0f;
}

void mtgb::RigidBody::OnCollisionEnter(const std::function<void(const EntityId)>& _onHit)
{
	isNeedUpdate = true;
	onHit_ = _onHit;
}

void mtgb::RigidBody::OnCollisionStay(const std::function<void(const EntityId)>& _onHit)
{
	isNeedUpdate = true;
	onStay_ = _onHit;
}

void mtgb::RigidBody::OnCollisionExit(const std::function<void(const EntityId)>& _onExit)
{
	isNeedUpdate = true;
	onExit_ = _onExit;
}

bool mtgb::RigidBody::IsJumping()
{
	return velocity.y > 0.0f;
}

Vector3 mtgb::RigidBody::GetPushAmount(const DirectX::BoundingSphere& _sphere, const DirectX::BoundingBox& _aabb)
{
	Vector3 aabbMin = _aabb.Center - _aabb.Extents;
	Vector3 aabbMax = _aabb.Center + _aabb.Extents;

	// 最短地点
	Vector3 closest;

	// 各座標軸にクランプする
	closest.x = std::clamp(_sphere.Center.x, aabbMin.x, aabbMax.x);
	closest.y = std::clamp(_sphere.Center.y, aabbMin.y, aabbMax.y);
	closest.z = std::clamp(_sphere.Center.z, aabbMin.z, aabbMax.z);

	Vector3 v = _sphere.Center - closest;
	float dist = v.Size();

	// 距離がほぼゼロの場合
	if (dist <= FLT_EPSILON)
	{
		// AABBの中心から球の中心への方向
		v = Vector3::Normalize(_sphere.Center - _aabb.Center);
		// 方向が決まらなければ押し出しはしない
		if (v.Size() <= FLT_EPSILON)
			return Vector3::Zero();
		dist = 0.0f;
	}
	else
	{
		v = Vector3::Normalize(v);
	}

	// 押し出し量
	float penetration = _sphere.Radius - dist;
	if (penetration <= 0.0f)
		return Vector3::Zero();

	// 最短地点から球の中心へ押し出す
	return v * penetration;
}


