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
	: isStatic_{ false }
	, colliderTag_{ ColliderTag::GAME_OBJECT }
	, pTransform_{&Transform::Get(_entityId)}
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
		break;
	}
}

void mtgb::Collider::UpdateBoundingSphere()
{
	computeSphere_.Center = pTransform_->position;
}

void mtgb::Collider::UpdateBoundingBox()
{
	if (!isStatic_)
	{
		computeBox_.Center = pTransform_->position;
		computeBox_.Extents.x = extents_.x * pTransform_->scale.x;
		computeBox_.Extents.y = extents_.y * pTransform_->scale.y;
		computeBox_.Extents.z = extents_.z * pTransform_->scale.z;
	}
}

bool mtgb::Collider::IsHit(const Collider& _other) const
{
	using DirectX::XMVector3TransformCoord;
	DirectX::ContainmentType containmentType = DirectX::DISJOINT;
	// ステージ同士は接触しないものとする
	if (colliderTag_ == ColliderTag::STAGE && _other.colliderTag_ == ColliderTag::STAGE)
	{
		return false;
	}

	if (colliderType_ == _other.colliderType_)
	{
		if (colliderType_ == ColliderType::TYPE_SPHERE)
		{
			//// 距離が双方の球の半径よりも小さければ当たっている
			containmentType = computeSphere_.Contains(_other.computeSphere_);
		}
		else if (colliderType_ == ColliderType::TYPE_AABB)
		{
			containmentType = computeBox_.Contains(_other.computeBox_);
		}
	}
	else
	{
		if (colliderType_ == ColliderType::TYPE_SPHERE)
		{	
			containmentType = computeSphere_.Contains(_other.computeBox_);
		}
		else if (colliderType_ == ColliderType::TYPE_AABB)
		{
			containmentType = computeBox_.Contains(_other.computeSphere_);
		}
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

bool mtgb::Collider::IsHit(const Vector3& _origin, const Vector3& _dir, float* dist)
{
	switch (colliderType_)
	{
	case ColliderType::TYPE_SPHERE:
		return IsHit(computeSphere_, _origin, _dir, dist);
		break;
	case ColliderType::TYPE_AABB:
		 return IsHit(computeBox_,_origin,_dir,dist);
		break;
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
	else
	{
		computeSphere_.Center = _center;
	}
	center_ = _center;
}

void mtgb::Collider::SetExtents(const Vector3& _extents)
{
	computeBox_.Extents.x = _extents.x * pTransform_->scale.x;
	computeBox_.Extents.y = _extents.y * pTransform_->scale.y;
	computeBox_.Extents.z = _extents.z * pTransform_->scale.z;
	extents_ = _extents;
}

void mtgb::Collider::SetRadius(float _radius)
{
	computeSphere_.Radius = _radius * pTransform_->scale.x;
	radius_ = _radius;
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

void mtgb::Collider::Push(const Collider& _other)
{
	std::optional<DirectX::BoundingBox> aabb;
	std::optional<DirectX::BoundingSphere> sphere;

	EntityId sphereTypeEntityId = INVALID_ENTITY;

	// 球とAABBのみ押し出しを実装しているを実装している
	if (colliderType_ == ColliderType::TYPE_SPHERE && _other.colliderType_ == ColliderType::TYPE_AABB)
	{
		sphereTypeEntityId = GetEntityId();
		sphere = computeSphere_;
		aabb = _other.computeBox_;
	}
	else if (_other.colliderType_ == ColliderType::TYPE_SPHERE && colliderType_ == ColliderType::TYPE_AABB)
	{
		sphereTypeEntityId = _other.GetEntityId();
		sphere = _other.computeSphere_;
		aabb = computeBox_;
	}
	else
	{
		return;
	}
	Transform& transform = Transform::Get(sphereTypeEntityId);

	std::optional<IntersectInfo> info = Intersect(sphere.value(), aabb.value());

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
		copyTransform.scale *= Vector3::One() * computeSphere_.Radius;
		Draw::FBXModel(hSphereModel_, copyTransform, 0,ShaderType::Debug3D);
		break;
	case ColliderType::TYPE_CAPSULE:
		break;
	case ColliderType::TYPE_AABB:


		if (!isStatic_)
		{
			copyTransform = *pTransform_;
		}
		else
		{
			copyTransform.parent = INVALID_ENTITY;
		}
		
		// 軸並行なので回転はなし
		copyTransform.rotate = Quaternion{};

		if (isStatic_)
		{
			// 静的、StatefulTransform不要なのでそのまま代入
			copyTransform.position = computeBox_.Center;
			copyTransform.scale = computeBox_.Extents * 2.0f;
		}
		else
		{
			// StatefulTransformに合わせて位置、サイズを調整
			copyTransform.scale = computeBox_.Extents * 2.0f;
		}
		
		//copyTransform.Compute();
		Draw::FBXModel(hBoxModel_, copyTransform, 0, ShaderType::Debug3D);
		break;
	default:
		break;
	}
}

mtgb::FBXModelHandle mtgb::Collider::hSphereModel_{ mtgb::INVALID_HANDLE };
mtgb::FBXModelHandle mtgb::Collider::hBoxModel_{ mtgb::INVALID_HANDLE };


