#include "Collider.h"
#include <DirectXMath.h>
#include "Matrix4x4.h"
#include "Draw.h"
#include "Debug.h"
#include "Transform.h"
#include "RigidBody.h"
#include "Intersection.h"
#include "Fbx.h"
mtgb::Collider::Collider(EntityId _entityId)
	: IComponent(_entityId)
	, colliderType_ { ColliderType::TYPE_SPHERE }
	, isStatic_ { false }
	, colliderTag_ { ColliderTag::GAME_OBJECT }
	, isTrigger_ { false }
	, pTransform_ { &Transform::Get(_entityId) }
	, center_ { 0.0f, 0.0f, 0.0f }
	, radius_ { 1.0f }
	, extents_ { 0.5f, 0.5f, 0.5 }
{
}

mtgb::Collider::Collider(EntityId _entityId, ColliderTag _colliderTag)
	: Collider(_entityId)
{
	colliderTag_ = _colliderTag;

	switch (_colliderTag)
	{
			// 現在はゲームオブジェクトは動的、ステージは静的と断定しているが
			// 動的なステージなども追加されるかもしれないので注意
		case ColliderTag::GAME_OBJECT :
			isStatic_	= false;
			pTransform_ = &Transform::Get(_entityId);
			break;
		case ColliderTag::STAGE :
			isStatic_	= true;
			pTransform_ = nullptr;
			break;
	}
}

mtgb::Collider::~Collider() {}

mtgb::Collider& mtgb::Collider::operator=(const Collider& _other)
{
	if (this == &_other)
	{
		return *this;
	}
	IComponent::operator=(_other);
	this->colliderType_ = _other.colliderType_;
	this->isStatic_		= _other.isStatic_;
	this->colliderTag_	= _other.colliderTag_;
	this->isTrigger_	= _other.isTrigger_;
	this->center_		= _other.center_;
	this->radius_		= _other.radius_;
	this->extents_		= _other.extents_;
	this->pTransform_	= _other.pTransform_;
	return *this;
}

void mtgb::Collider::UpdateBoundingData()
{
	switch (colliderType_)
	{
		case ColliderType::TYPE_SPHERE :
			UpdateBoundingSphere();
			break;
		case ColliderType::TYPE_AABB :
		case ColliderType::TYPE_OBB :
			UpdateBoundingBox();
			break;
	}
}

void mtgb::Collider::HandleCollision(const Collider& _other)
{
	if (isTrigger_ || _other.isTrigger_)
		return;

	RigidBody* pRb = nullptr;

	bool isExistingRigidBody = Game::System<RigidBodyCP>().TryGet(pRb, _other.GetEntityId());
	if (isExistingRigidBody)
	{
		if (pRb->isKinematic_ == false)
		{
			Push(_other);
		}
	}
}

void mtgb::Collider::UpdateBoundingSphere()
{
	computeSphere_.Center = pTransform_->position + center_;
	float maxScale		  = (std::max)(pTransform_->scale.x, pTransform_->scale.y);
	maxScale			  = (std::max)(pTransform_->scale.z, maxScale);
	computeSphere_.Radius = maxScale * radius_;
}

void mtgb::Collider::UpdateBoundingBox()
{
	if (!isStatic_)
	{
		if (colliderType_ == ColliderType::TYPE_AABB)
		{
			computeBox_.Center	  = pTransform_->position + center_;
			computeBox_.Extents.x = extents_.x * pTransform_->scale.x;
			computeBox_.Extents.y = extents_.y * pTransform_->scale.y;
			computeBox_.Extents.z = extents_.z * pTransform_->scale.z;
		}
		else if (colliderType_ == ColliderType::TYPE_OBB)
		{
			computeOBB_.Center	  = pTransform_->position + center_;
			computeOBB_.Extents.x = extents_.x * pTransform_->scale.x;
			computeOBB_.Extents.y = extents_.y * pTransform_->scale.y;
			computeOBB_.Extents.z = extents_.z * pTransform_->scale.z;
			DirectX::XMStoreFloat4(&computeOBB_.Orientation, pTransform_->rotate);
		}
	}
}

bool mtgb::Collider::IsHit(const Collider& _other) const
{
	DirectX::ContainmentType containmentType = DirectX::DISJOINT;
	// ステージ同士は接触しないものとする
	if (colliderTag_ == ColliderTag::STAGE && _other.colliderTag_ == ColliderTag::STAGE)
	{
		return false;
	}

	switch (colliderType_)
	{
		case ColliderType::TYPE_SPHERE :
			switch (_other.colliderType_)
			{
				case ColliderType::TYPE_SPHERE :
					containmentType = computeSphere_.Contains(_other.computeSphere_);
					break;
				case ColliderType::TYPE_AABB :
					containmentType = computeSphere_.Contains(_other.computeBox_);
					break;
				case ColliderType::TYPE_OBB :
					containmentType = computeSphere_.Contains(_other.computeOBB_);
					break;
			}
			break;
		case ColliderType::TYPE_AABB :
			switch (_other.colliderType_)
			{
				case ColliderType::TYPE_SPHERE :
					containmentType = computeBox_.Contains(_other.computeSphere_);
					break;
				case ColliderType::TYPE_AABB :
					containmentType = computeBox_.Contains(_other.computeBox_);
					break;
				case ColliderType::TYPE_OBB :
					containmentType = computeBox_.Contains(_other.computeOBB_);
					break;
			}
			break;
		case ColliderType::TYPE_OBB :

			switch (_other.colliderType_)
			{
				case ColliderType::TYPE_SPHERE :
					containmentType = computeOBB_.Contains(_other.computeSphere_);
					break;
				case ColliderType::TYPE_AABB :
					containmentType = computeOBB_.Contains(_other.computeBox_);
					break;
				case ColliderType::TYPE_OBB :
					containmentType = computeOBB_.Contains(_other.computeOBB_);
					break;
			}
			break;
	}

	if (containmentType == DirectX::INTERSECTS || containmentType == DirectX::CONTAINS)
	{
		return true;
	}

	return false;
}

bool mtgb::Collider::IsHit(
	const Vector3& _origin,
	const Vector3& _dir,
	float _maxDistance,
	Intersection::RaycastInfo* _info
) const
{
	switch (colliderType_)
	{
		case ColliderType::TYPE_SPHERE :
			return Intersection::IsHit(computeSphere_, _origin, _dir, _maxDistance, _info);
			break;
		case ColliderType::TYPE_AABB :
			return Intersection::IsHit(computeBox_, _origin, _dir, _maxDistance, _info);
			break;
		case ColliderType::TYPE_OBB :
			return Intersection::IsHit(computeOBB_, _origin, _dir, _maxDistance, _info);
		default :
			return false;
	}
}

bool mtgb::Collider::IsHit(const Vector3& _center, float _radius) const
{
	static Matrix4x4 matrix {};

	if (colliderType_ == ColliderType::TYPE_SPHERE)
	{
		pTransform_->GenerateWorldMatrix(&matrix);
		Vector3 worldPosition { Vector3(computeSphere_.Center) * matrix };

		// 引数で球を作る

		float distance { (_center - worldPosition).Size() };
		float hitDistance { computeSphere_.Radius + _radius };

		// 距離が双方の球の半径よりも小さければ当たっている
		return (distance <= hitDistance);
	}

	return false;
}

void mtgb::Collider::ForEachCollisionEnter(const EntityCallback& _func)
{
	for (Collider* onCollider : onColliders_)
	{
		if (onCollidersPrev_.find(onCollider) == onCollidersPrev_.end())
		{
			// 以前は衝突してない
			_func(onCollider->GetEntityId());
		}
	}
}

void mtgb::Collider::ForEachCollisionStay(const EntityCallback& _func)
{
	for (Collider* onCollider : onColliders_)
	{
		if (onCollidersPrev_.find(onCollider) == onCollidersPrev_.end())
			continue;

		// 以前から衝突している
		_func(onCollider->GetEntityId());
	}
}

void mtgb::Collider::ForEachCollisionExit(const EntityCallback& _func)
{
	for (Collider* onColliderPrev : onCollidersPrev_)
	{
		if (onColliders_.find(onColliderPrev) != onColliders_.end())
			continue;

		// 以前は衝突していて、現在はしていない
		_func(onColliderPrev->GetEntityId());
	}
}

void mtgb::Collider::SetCenter(const Vector3& _center)
{
	if (colliderType_ == ColliderType::TYPE_AABB)
	{
		computeBox_.Center = _center;
	}
	else if (colliderType_ == ColliderType::TYPE_AABB)
	{
		computeSphere_.Center = _center;
	}
	else if (colliderType_ == ColliderType::TYPE_OBB)
	{
		computeOBB_.Center = _center;
	}
	center_ = _center;
}

void mtgb::Collider::SetExtents(const Vector3& _extents)
{
	extents_ = _extents;
	UpdateBoundingBox();
}

void mtgb::Collider::SetRadius(float _radius)
{
	radius_ = _radius;
	UpdateBoundingSphere();
}

mtgb::Vector3 mtgb::Collider::GetCenter() const
{
	return center_;
}

mtgb::Vector3 mtgb::Collider::GetExtents() const
{
	if (colliderType_ == ColliderType::TYPE_AABB)
	{
		return computeBox_.Extents;
	}
	if (colliderType_ == ColliderType::TYPE_OBB)
	{
		return computeOBB_.Extents;
	}
	LOGIMGUI("colliderType is not AABB");
	return Vector3::Zero();
}

float mtgb::Collider::GetRadius() const
{
	if (colliderType_ == ColliderType::TYPE_SPHERE)
	{
		return computeSphere_.Radius;
	}
	LOGIMGUI("colliderType is not Sphere");
	return 0.0f;
}

void mtgb::Collider::Push(const Collider& _other)
{
	std::optional<Intersection::IntersectInfo> info = std::nullopt;

	EntityId sphereTypeEntityId = INVALID_ENTITY;

	switch (colliderType_)
	{
		case ColliderType::TYPE_SPHERE :
			switch (_other.colliderType_)
			{
				case ColliderType::TYPE_AABB :
					info = Intersection::Intersect(computeSphere_, _other.computeBox_);
					break;
				case ColliderType::TYPE_OBB :
					info = Intersection::Intersect(computeSphere_, _other.computeOBB_);
					break;
			}
			sphereTypeEntityId = GetEntityId();
			break;
		case ColliderType::TYPE_AABB :
			switch (_other.colliderType_)
			{
				case ColliderType::TYPE_SPHERE :
					info = Intersection::Intersect(_other.computeSphere_, computeBox_);
					break;
			}
			sphereTypeEntityId = _other.GetEntityId();
			break;
		case ColliderType::TYPE_OBB :
			switch (_other.colliderType_)
			{
				case ColliderType::TYPE_SPHERE :
					info = Intersection::Intersect(_other.computeSphere_, computeOBB_);
					break;
			}
			sphereTypeEntityId = _other.GetEntityId();
			break;
	}

	Transform& transform = Transform::Get(sphereTypeEntityId);

	if (info.has_value() == false)
		return;

	if (info.value().closest.y < transform.position.y)
	{
		RigidBody& rigidBody = RigidBody::Get(sphereTypeEntityId);
		rigidBody.OnGround();
	}
	transform.position += info.value().push;
}

void mtgb::Collider::OnPostRestore()
{
	// TODO: コンポーネントの復元に依存関係を設定する
	// 現在Transformよりも先に復元されてしまうためscaleの反映ができず、
	// 次の更新時になってしまう
	SetCenter(center_);
	switch (colliderType_)
	{
		case ColliderType::TYPE_SPHERE :
			SetRadius(radius_);
			break;
		case ColliderType::TYPE_AABB :
		case ColliderType::TYPE_OBB :
			SetExtents(extents_);
			break;
	}
	pTransform_ = &Transform::Get(GetEntityId());
}

void mtgb::Collider::Reset()
{
	onColliders_.clear();
	onCollidersPrev_.clear();
}

void mtgb::Collider::OnChangeEntityId()
{
	pTransform_ = &Transform::Get(GetEntityId());
}

void mtgb::Collider::Draw() const
{
	static Transform copyTransform {};

	switch (colliderType_)
	{
		case ColliderType::TYPE_SPHERE :
			copyTransform = *pTransform_;
			copyTransform.position += center_;
			copyTransform.scale *= Vector3::One() * computeSphere_.Radius;
			copyTransform.Compute();
			Game::System<mtgb::Fbx>().Draw(hSphereModel_, copyTransform, 0, ShaderType::DEBUG3_D);
			break;
		case ColliderType::TYPE_AABB :

			if (isStatic_)
			{
				copyTransform.parent = INVALID_ENTITY;
			}
			else
			{
				copyTransform = *pTransform_;
			}

			// 軸並行なので回転はなし
			copyTransform.rotate = Quaternion {};

			if (isStatic_)
			{
				// 静的、Transform不要なのでそのまま代入
				copyTransform.position = computeBox_.Center;
				copyTransform.scale	   = computeBox_.Extents * 2.0f;
			}
			else
			{
				// Transformに合わせて位置、サイズを調整
				copyTransform.position += center_;
				copyTransform.scale = computeBox_.Extents * 2.0f;
			}
			copyTransform.Compute();
			Game::System<mtgb::Fbx>().Draw(hBoxModel_, copyTransform, 0, ShaderType::DEBUG3_D);
			break;
		case ColliderType::TYPE_OBB :
			if (isStatic_)
			{
				copyTransform.parent = INVALID_ENTITY;
			}
			else
			{
				copyTransform = *pTransform_;
			}

			if (isStatic_)
			{
				// 静的、Transform不要なのでそのまま代入
				copyTransform.position = computeBox_.Center;
				copyTransform.scale	   = computeBox_.Extents * 2.0f;
			}
			else
			{
				// Transformに合わせて位置、サイズを調整
				copyTransform.position += center_;
				copyTransform.scale = computeBox_.Extents * 2.0f;
			}
			copyTransform.Compute();
			Game::System<mtgb::Fbx>().Draw(hBoxModel_, copyTransform, 0, ShaderType::DEBUG3_D);
			break;
		default :
			break;
	}
}

mtgb::FBXModelHandle mtgb::Collider::hSphereModel_ { mtgb::INVALID_HANDLE };
mtgb::FBXModelHandle mtgb::Collider::hBoxModel_ { mtgb::INVALID_HANDLE };
