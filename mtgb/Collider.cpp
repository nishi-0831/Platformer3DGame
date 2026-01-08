#include "Collider.h"
#include "DirectXMath.h"
#include "Matrix4x4.h"
#include "Draw.h"
#include "Debug.h"
#include "Transform.h"
#include <cfloat>
#include "RigidBody.h"
namespace
{
	mtgb::Matrix4x4 matrix{};
	DirectX::XMVECTORF32 unitVectorEpsilon{ FLT_EPSILON ,FLT_EPSILON ,FLT_EPSILON ,FLT_EPSILON };
	bool XMVECTORIsUnit(DirectX::FXMVECTOR _v)
	{
		DirectX::XMVECTOR difference = DirectX::XMVectorSubtract(DirectX::XMVector3Length(_v), DirectX::XMVectorSplatOne());
		return DirectX::XMVector4Less(DirectX::XMVectorAbs(difference), unitVectorEpsilon);	
	}
}

mtgb::Collider::Collider(EntityId _entityId)
	: IComponent(_entityId)
	, colliderType_{ ColliderType::TYPE_SPHERE }
	, isStatic_{ false }
	, colliderTag_{ ColliderTag::GAME_OBJECT }
	, isTrigger_{ false }
	, pTransform_{ &Transform::Get(_entityId) }
	, center_{ 0.0f,0.0f,0.0f }
	, radius_{ 1.0f }
	, extents_{ 0.5f,0.5f ,0.5 }
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
	case ColliderTag::GAME_OBJECT:
		isStatic_ = false;
		pTransform_ = &Transform::Get(_entityId);
		break;
	case ColliderTag::STAGE:
		isStatic_ = true;
		pTransform_ = nullptr;
		break;
	}
	
}

mtgb::Collider::~Collider()
{
}

Collider& mtgb::Collider::operator=(const Collider& _other)
{
	if (this == &_other)
	{
		return *this;
	}
	this->colliderType_ = _other.colliderType_;
	this->isStatic_ = _other.isStatic_;
	this->colliderTag_ = _other.colliderTag_;
	this->isTrigger_ = _other.isTrigger_;

	*(this->pTransform_) = *(_other.pTransform_);
	this->center_ = _other.center_;
	this->radius_ = _other.radius_;
	this->extents_ = _other.extents_;

	return *this;
}

void mtgb::Collider::UpdateBoundingData()
{
	switch (colliderType_)
	{
	case ColliderType::TYPE_SPHERE:
		UpdateBoundingSphere();
		break;
	case ColliderType::TYPE_AABB:
		UpdateBoundingBox();
		break;
	case ColliderType::TYPE_CAPSULE:
		// TODO: カプセル初期化
	case ColliderType::TYPE_OBB:
		computeOBB_.Center = pTransform_->position + center_;
		computeOBB_.Extents.x = extents_.x * pTransform_->scale.x;
		computeOBB_.Extents.y = extents_.y * pTransform_->scale.y;
		computeOBB_.Extents.z = extents_.z * pTransform_->scale.z;
		DirectX::XMStoreFloat4(&computeOBB_.Orientation, pTransform_->rotate);
		break;
	}
}

void mtgb::Collider::UpdateBoundingSphere()
{
	computeSphere_.Center = pTransform_->position + center_;
	float maxScale = (std::max)(pTransform_->scale.x, pTransform_->scale.y);
	maxScale = (std::max)(pTransform_->scale.z, maxScale);
	computeSphere_.Radius = maxScale * radius_;
}

void mtgb::Collider::UpdateBoundingBox()
{
	if (!isStatic_)
	{
		computeBox_.Center = pTransform_->position + center_;
		computeBox_.Extents.x = extents_.x * pTransform_->scale.x;
		computeBox_.Extents.y = extents_.y * pTransform_->scale.y;
		computeBox_.Extents.z = extents_.z * pTransform_->scale.z;
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
	case ColliderType::TYPE_SPHERE:
		switch (_other.colliderType_)
		{
		case ColliderType::TYPE_SPHERE:
			containmentType = computeSphere_.Contains(_other.computeSphere_);
			break;
		case ColliderType::TYPE_AABB:
			containmentType = computeSphere_.Contains(_other.computeBox_);
			break;
		case ColliderType::TYPE_OBB:
			containmentType = computeSphere_.Contains(_other.computeOBB_);
			break;
		}
		break;
	case ColliderType::TYPE_AABB:
		switch (_other.colliderType_)
		{
		case ColliderType::TYPE_SPHERE:
			containmentType = computeBox_.Contains(_other.computeSphere_);
			break;
		case ColliderType::TYPE_AABB:
			containmentType = computeBox_.Contains(_other.computeBox_);
			break;
		case ColliderType::TYPE_OBB:
			containmentType = computeBox_.Contains(_other.computeOBB_);
			break;
		}
		break;
	case ColliderType::TYPE_OBB:
		
		switch (_other.colliderType_)
		{
		case ColliderType::TYPE_SPHERE:
			containmentType = computeOBB_.Contains(_other.computeSphere_);
			break;
		case ColliderType::TYPE_AABB:
			containmentType = computeOBB_.Contains(_other.computeBox_);
			break;
		case ColliderType::TYPE_OBB:
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

bool mtgb::Collider::IsHit(const DirectX::BoundingSphere& _sphere, const Vector3& _origin, const Vector3& _dir, float* dist)
{
	/////
	// DirectXCollision.hのBoundingSphere::Intersectsをコピペした。
	// 何故かIntersectsに実引数が正常に渡されないから。
	/////
	using namespace DirectX;
	// まずBoundingSphereを最新状態に更新
	//const_cast<Collider*>(this)->UpdateBoundingData();
	
	// 方向ベクトルを正規化（元のベクトルは保持）
	Vector3 normalizedDir = Vector3::Normalize(_dir);
	
	XMVECTOR vNormalizeDir = XMLoadFloat3(&normalizedDir);
	if (!XMVECTORIsUnit(vNormalizeDir))
	{
		return false;
	}
	
	XMVECTOR vCenter =  XMLoadFloat3(&_sphere.Center);
	XMVECTOR vRadius =  XMVectorReplicatePtr(&_sphere.Radius);

	//球の中心からレイの原点へのベクトル
	XMVECTOR l = XMVectorSubtract(vCenter, _origin);

	//lをレイの方向に射影したスカラー
	//球の中心とレイの最も近い点の距離
	XMVECTOR s = XMVector3Dot(l, normalizedDir);

	//球の中心からレイの原点への距離の二乗
	XMVECTOR l2 = XMVector3Dot(l, l);

	//半径の二乗
	XMVECTOR r2 = XMVectorMultiply(vRadius, vRadius);

	//球の中心からレイへの垂線の二乗
	//三平方の定理でいうとlが斜辺にあたる
	
	XMVECTOR m2 = XMVectorNegativeMultiplySubtract(s, s, l2);
	
	
	XMVECTOR NoIntersection;

	//もしレイの原点が球の外側、かつ
	//球の中心がレイの原点の後ろにあるならば接触していない
	NoIntersection = XMVectorAndInt(
		//射影ベクトルが負ならレイの方向と反対、レイの原点より後ろにある
		XMVectorLess(s, XMVectorZero())
		//l2の方が大きいなら球の外側にある
		, XMVectorGreater(l2, r2));

	//球の中心からレイへの垂線が球の半径よりも大きいか
	//レイの最も近い地点が球の外側にあるか


	if (XMVector4Greater(m2, r2))
	{
		return false;
	}

	//衝突しているとして、最も近い地点を計算

	//球の中心とレイの最も近い点から球の表面までの距離
	XMVECTOR q = XMVectorSqrt(XMVectorSubtract(r2, m2));

	//r2-m2が負の値の場合はqに0を入れる
	XMVECTOR mask = XMVectorGreater(m2, r2);
	XMVECTOR zero = XMVectorZero();
	//maskがtrueの場合はzero、falseの場合はq
	q = XMVectorSelect(q, zero, mask);

	//レイの原点から球との交点までの距離
	//手前
	XMVECTOR t1 = XMVectorSubtract(s, q);
	//奥
	XMVECTOR t2 = XMVectorAdd(s, q);

	//レイの原点が球の内側か
	XMVECTOR originInside = XMVectorLessOrEqual(l2, r2);
	
	//第三引数がtrueなら第二引数、falseなら第一引数
	XMVECTOR t = XMVectorSelect(t1, t2, originInside);

	if (XMVector4NotEqualInt(NoIntersection, XMVectorTrueInt()))
	{
		DirectX::XMStoreFloat(dist, t);
		return true;
	}

	return false;
}

bool mtgb::Collider::IsHit(const DirectX::BoundingBox& _aabb, const Vector3& _origin, const Vector3& _dir, float* dist)
{
	return _aabb.Intersects(_origin, _dir, *dist);
}

bool mtgb::Collider::IsHit(const DirectX::BoundingOrientedBox& _obb, const Vector3& _origin, const Vector3& _dir, float* dist)
{
	return _obb.Intersects(_origin, _dir, *dist);
}

bool mtgb::Collider::IsHit(const Vector3& _origin, const Vector3& _dir, float* _dist)
{
	switch (colliderType_)
	{
	case ColliderType::TYPE_SPHERE:
		return IsHit(computeSphere_, _origin, _dir, _dist);
		break;
	case ColliderType::TYPE_AABB:
		 return IsHit(computeBox_,_origin,_dir,_dist);
		break;
	case ColliderType::TYPE_OBB:
		return IsHit(computeOBB_, _origin, _dir, _dist);
	default:
		return false;
	}

	return false;
}

bool mtgb::Collider::IsHit(const Vector3& _center, float _radius) const
{
	static Matrix4x4 matrix{};

	if (colliderType_ == ColliderType::TYPE_SPHERE)
	{
		pTransform_->GenerateWorldMatrix(&matrix);
		Vector3 worldPosition{ Vector3(computeSphere_.Center) * matrix };

		// 引数で球を作る

		float distance{ (_center - worldPosition).Size() };
		float hitDistance{ computeSphere_.Radius + _radius };

		// 距離が双方の球の半径よりも小さければ当たっている
		return (distance <= hitDistance);
	}
	else if (colliderType_ == ColliderType::TYPE_CAPSULE)
	{
		// TODO: カプセルと球の当たり判定

	}

	return false;
}

void mtgb::Collider::SetCenter(const Vector3& _center)
{
	if (colliderType_ == ColliderType::TYPE_AABB)
	{
		computeBox_.Center = _center;
	}
	else if(colliderType_ == ColliderType::TYPE_AABB)
	{
		computeSphere_.Center = _center;
	}
	else if(colliderType_ == ColliderType::TYPE_OBB)
	{
		computeOBB_.Center = _center;
	}
	center_ = _center;
}

void mtgb::Collider::SetExtents(const Vector3& _extents)
{
	if (colliderType_ == ColliderType::TYPE_AABB)
	{
		computeBox_.Extents.x = _extents.x * pTransform_->scale.x;
		computeBox_.Extents.y = _extents.y * pTransform_->scale.y;
		computeBox_.Extents.z = _extents.z * pTransform_->scale.z;
	}
	else if (colliderType_ == ColliderType::TYPE_OBB)
	{
		computeOBB_.Extents.x = _extents.x * pTransform_->scale.x;
		computeOBB_.Extents.y = _extents.y * pTransform_->scale.y;
		computeOBB_.Extents.z = _extents.z * pTransform_->scale.z;
	}
	extents_ = _extents;
}

void mtgb::Collider::SetRadius(float _radius)
{
	computeSphere_.Radius = _radius * pTransform_->scale.x;
	radius_ = _radius;
}

Vector3 mtgb::Collider::GetCenter()
{
	return center_;
}

Vector3 mtgb::Collider::GetExtents()
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

float mtgb::Collider::GetRadius()
{
	if (colliderType_ == ColliderType::TYPE_SPHERE)
	{
		return computeSphere_.Radius;
	}
	LOGIMGUI("colliderType is not Sphere");
	return 0.0f;
}

std::optional<IntersectInfo> mtgb::Collider::Intersect(const DirectX::BoundingSphere& _sphere, const DirectX::BoundingBox& _aabb)
{
	IntersectInfo info;

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
			return std::nullopt;
		dist = 0.0f;
	}
	else
	{
		v = Vector3::Normalize(v);
	}

	// 押し出し量
	float penetration = _sphere.Radius - dist;
	if (penetration <= 0.0f)
		return std::nullopt;

	info.closest = closest;
	// 最短地点から球の中心へ押し出す
	info.push = v * penetration;
	return info;
}

std::optional<IntersectInfo> mtgb::Collider::Intersect(const DirectX::BoundingSphere& _sphere, const DirectX::BoundingOrientedBox& _obb)
{
	IntersectInfo info;

	Vector3 localCenter = _sphere.Center - _obb.Center;
	Vector3 extents = _obb.Extents;
	Quaternion rotate = _obb.Orientation;
	Matrix4x4 rotMat = DirectX::XMMatrixRotationQuaternion(rotate);
	Vector3 rotatedExtents = DirectX::XMVector3TransformCoord(extents,rotMat);
	rotatedExtents.x = std::abs(rotatedExtents.x);
	rotatedExtents.y = std::abs(rotatedExtents.y);
	rotatedExtents.z = std::abs(rotatedExtents.z);

	Vector3 closest;
	closest.x = std::clamp(localCenter.x, -rotatedExtents.x, rotatedExtents.x);
	closest.y = std::clamp(localCenter.y, -rotatedExtents.y, rotatedExtents.y);
	closest.z = std::clamp(localCenter.z, -rotatedExtents.z, rotatedExtents.z);

	Vector3 v = localCenter - closest;
	float distance = v.Size();

	if (distance <= FLT_EPSILON)
	{
		v = Vector3::Normalize(_sphere.Center - _obb.Center);
		// 方向が決まらなければ押し出しはしない
		if (v.Size() <= FLT_EPSILON)
			return std::nullopt;
		distance = 0.0f;
	}

	float penetration = _sphere.Radius - distance;
	if (penetration <= 0.0f)
		return std::nullopt;

	info.closest = closest;
	info.push = Vector3::Normalize(v) * penetration;
	return info;
}

void mtgb::Collider::Push(const Collider& _other)
{
	std::optional<DirectX::BoundingBox> aabb;
	std::optional<DirectX::BoundingSphere> sphere;
	std::optional<IntersectInfo> info = std::nullopt;

	EntityId sphereTypeEntityId = INVALID_ENTITY;

	switch (colliderType_)
	{
	case ColliderType::TYPE_SPHERE:
		switch (_other.colliderType_)
		{
		case ColliderType::TYPE_AABB:
			info = Intersect(computeSphere_, _other.computeBox_);
			break;
		case ColliderType::TYPE_OBB:
			info = Intersect(computeSphere_, _other.computeOBB_);
			break;
		}
		sphereTypeEntityId = GetEntityId();
		break;
	case ColliderType::TYPE_AABB:
		switch (_other.colliderType_)
		{
		case ColliderType::TYPE_SPHERE:
			info = Intersect(_other.computeSphere_,computeBox_);
			break;
		}
		sphereTypeEntityId = _other.GetEntityId();
		break;
	case ColliderType::TYPE_OBB:
		switch (_other.colliderType_)
		{
		case ColliderType::TYPE_SPHERE:
			info = Intersect(_other.computeSphere_, computeOBB_);
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
	case ColliderType::TYPE_SPHERE:
	case ColliderType::TYPE_CAPSULE:
		SetRadius(radius_);
		break;
	case ColliderType::TYPE_AABB:
	case ColliderType::TYPE_OBB:
		SetExtents(extents_);
		break;
	}
}

void mtgb::Collider::Draw() const
{
	static Transform copyTransform{};


	switch (colliderType_)
	{
	case ColliderType::TYPE_SPHERE:
		copyTransform = *pTransform_;
		copyTransform.position += center_;
		copyTransform.scale *= Vector3::One() * computeSphere_.Radius;
		copyTransform.Compute();
		Draw::FBXModel(hSphereModel_, copyTransform, 0,ShaderType::Debug3D);
		break;
	case ColliderType::TYPE_CAPSULE:
		break;
	case ColliderType::TYPE_AABB:

		if (isStatic_)
		{
			copyTransform.parent = INVALID_ENTITY;
		}
		else
		{
			copyTransform = *pTransform_;
		}
		
		// 軸並行なので回転はなし
		copyTransform.rotate = Quaternion{};

		if (isStatic_)
		{
			// 静的、Transform不要なのでそのまま代入
			copyTransform.position = computeBox_.Center;
			copyTransform.scale = computeBox_.Extents * 2.0f;
		}
		else
		{
			// Transformに合わせて位置、サイズを調整
			copyTransform.position += center_;
			copyTransform.scale = computeBox_.Extents * 2.0f;
		}
		copyTransform.Compute();
		Draw::FBXModel(hBoxModel_, copyTransform, 0, ShaderType::Debug3D);
		break;
	case ColliderType::TYPE_OBB:
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
			copyTransform.scale = computeBox_.Extents * 2.0f;
		}
		else
		{
			// Transformに合わせて位置、サイズを調整
			copyTransform.position += center_;
			copyTransform.scale = computeBox_.Extents * 2.0f;
		}
		copyTransform.Compute();
		Draw::FBXModel(hBoxModel_, copyTransform, 0, ShaderType::Debug3D);
		break;
	default:
		break;
	}
}

mtgb::FBXModelHandle mtgb::Collider::hSphereModel_{ mtgb::INVALID_HANDLE };
mtgb::FBXModelHandle mtgb::Collider::hBoxModel_{ mtgb::INVALID_HANDLE };


