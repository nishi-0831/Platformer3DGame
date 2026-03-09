#include "RigidBody.h"
#include "Transform.h"
#include <DirectXCollision.h>
#include <tuple>
#include <vector>
#include "GameTime.h"
namespace
{
	// TODO:重力を外部から設定可能にする
	float gravity { -20.0f };
} // namespace
mtgb::RigidBody::RigidBody(EntityId _entityId)
	: IComponent { _entityId }
	, useGravity_ { false }
	, onHit_ { [](EntityId) {} }
	, pTransform_ { &Transform::Get(_entityId) }
{
}

mtgb::RigidBody::~RigidBody() {}

mtgb::RigidBody& mtgb::RigidBody::operator=(const RigidBody& _other)
{
	if (&_other == this)
	{
		return *this;
	}

	this->velocity_		 = _other.velocity_;
	this->onHit_		 = _other.onHit_;
	this->onStay_		 = _other.onStay_;
	this->onExit_		 = _other.onExit_;
	this->useGravity_	 = _other.useGravity_;
	*(this->pTransform_) = *(_other.pTransform_);
	this->isGround_		 = _other.isGround_;
	this->isKinematic_	 = _other.isKinematic_;

	return *this;
}

void mtgb::RigidBody::UpdateVelocity()
{
	if (useGravity_)
	{
		velocity_ += Vector3::Up() * gravity * Time::DeltaTimeF();
	}
	pTransform_->position += velocity_ * Time::DeltaTimeF();
	if (FLT_EPSILON <= std::abs(velocity_.y))
	{
		isGround_ = false;
	}
}

void mtgb::RigidBody::OnGround()
{
	isGround_	= true;
	velocity_.y = (std::max)(velocity_.y, 0.0f);
}

bool mtgb::RigidBody::IsJumping()
{
	return velocity_.y > 0.0f;
}

mtgb::Vector3 mtgb::RigidBody::GetPushAmount(const DirectX::BoundingSphere& _sphere, const DirectX::BoundingBox& _aabb)
{
	Vector3 aabbMin = _aabb.Center - _aabb.Extents;
	Vector3 aabbMax = _aabb.Center + _aabb.Extents;

	// 最短地点
	Vector3 closest;

	// 各座標軸にクランプする
	closest.x = std::clamp(_sphere.Center.x, aabbMin.x, aabbMax.x);
	closest.y = std::clamp(_sphere.Center.y, aabbMin.y, aabbMax.y);
	closest.z = std::clamp(_sphere.Center.z, aabbMin.z, aabbMax.z);

	Vector3 v  = _sphere.Center - closest;
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

float mtgb::RigidBody::GetGravity()
{
	return gravity;
}
