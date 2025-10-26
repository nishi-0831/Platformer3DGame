#include "Collider.h"
#include "ColliderMemento.h"
#include "DirectXMath.h"
#include "Matrix4x4.h"
#include "Draw.h"
#include "Debug.h"
#include <cfloat>
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
	: StatefulComponent{_entityId}
	, ColliderData{.isStatic = false,.colliderTag = ColliderTag::GAME_OBJECT}
	, pTransform_{&Transform::Get(_entityId)}
{
}


mtgb::Collider::Collider(EntityId _entityId, ColliderTag _colliderTag)
	: StatefulComponent{ _entityId }
{
	colliderTag = _colliderTag;

	switch (_colliderTag)
	{
		// ���݂̓Q�[���I�u�W�F�N�g�͓��I�A�X�e�[�W�͐ÓI�ƒf�肵�Ă��邪
		// ���I�ȃX�e�[�W�Ȃǂ��ǉ�����邩������Ȃ��̂Œ���
	case ColliderTag::GAME_OBJECT:
		isStatic = false;
		pTransform_ = &Transform::Get(_entityId);
		break;
	case ColliderTag::STAGE:
		isStatic = true;
		pTransform_ = nullptr;
		break;
	}
	
}

mtgb::Collider::~Collider()
{
}

void mtgb::Collider::UpdateBoundingData()
{
	switch (type)
	{
	case ColliderType::TYPE_SPHERE:
		UpdateBoundingSphere();
		break;
	case ColliderType::TYPE_AABB:
		UpdateBoundingBox();
		break;
	case ColliderType::TYPE_CAPSULE:
		// TODO: �J�v�Z��������
		break;
	}
}

void mtgb::Collider::UpdateBoundingSphere()
{
	// StatefulTransform���猻�݂̈ʒu���擾����BoundingSphere���X�V
	
	//pTransform_->GenerateWorldMatrix(&matrix);
		
	//computeSphere_.Center = pTransform_->position + computeSphere_.Center;
	//computeSphere_.Center = Vector3(computeSphere_.Center) * matrix;
	computeSphere_.Center = pTransform_->position;
}

void mtgb::Collider::UpdateBoundingBox()
{
	if (!isStatic)
	{
		/*pTransform_->GenerateWorldMatrix(&matrix);
		computeBox_.Center = Vector3(computeBox_.Center) * matrix;*/
		computeBox_.Center = pTransform_->position;
	}
}

bool mtgb::Collider::IsHit(const Collider& _other) const
{
	using DirectX::XMVector3TransformCoord;

	// �X�e�[�W���m�͐ڐG���Ȃ����̂Ƃ���
	if (colliderTag == ColliderTag::STAGE && _other.colliderTag == ColliderTag::STAGE)
	{
		return false;
	}

	if (type == _other.type)
	{
		if (type == ColliderType::TYPE_SPHERE)
		{
			//// �������o���̋��̔��a������������Γ������Ă���
			return computeSphere_.Intersects(_other.computeSphere_);
		}
		else if (type == ColliderType::TYPE_AABB)
		{
			return computeBox_.Intersects(_other.computeBox_);
		}
	}
	else
	{
		if (type == ColliderType::TYPE_SPHERE)
		{	
			return computeSphere_.Intersects(_other.computeBox_);
		}
		else if (type == ColliderType::TYPE_AABB)
		{
			return computeBox_.Intersects(_other.computeSphere_);
		}
	}

	return false;
}

bool mtgb::Collider::IsHit(const Vector3& _origin, const Vector3& _dir, float* dist)
{
	/////
	// DirectXCollision.h��BoundingSphere::Intersects���R�s�y�����B
	// ���̂�Intersects�Ɏ�����������ɓn����Ȃ�����B
	/////
	using namespace DirectX;
	// �܂�BoundingSphere���ŐV��ԂɍX�V
	//const_cast<Collider*>(this)->UpdateBoundingData();
	
	// �����x�N�g���𐳋K���i���̃x�N�g���͕ێ��j
	Vector3 normalizedDir = Vector3::Normalize(_dir);
	
	XMVECTOR vNormalizeDir = XMLoadFloat3(&normalizedDir);
	if (!XMVECTORIsUnit(vNormalizeDir))
	{
		return false;
	}
	
	XMVECTOR vCenter =  XMLoadFloat3(&computeSphere_.Center);
	XMVECTOR vRadius =  XMVectorReplicatePtr(&computeSphere_.Radius);

	//���̒��S���烌�C�̌��_�ւ̃x�N�g��
	XMVECTOR l = XMVectorSubtract(vCenter, _origin);

	//l�����C�̕����Ɏˉe�����X�J���[
	//���̒��S�ƃ��C�̍ł��߂��_�̋���
	XMVECTOR s = XMVector3Dot(l, normalizedDir);

	//���̒��S���烌�C�̌��_�ւ̋����̓��
	XMVECTOR l2 = XMVector3Dot(l, l);

	//���a�̓��
	XMVECTOR r2 = XMVectorMultiply(vRadius, vRadius);

	//���̒��S���烌�C�ւ̐����̓��
	//�O�����̒藝�ł�����l���Εӂɂ�����
	
	XMVECTOR m2 = XMVectorNegativeMultiplySubtract(s, s, l2);
	
	
	XMVECTOR NoIntersection;

	//�������C�̌��_�����̊O���A����
	//���̒��S�����C�̌��_�̌��ɂ���Ȃ�ΐڐG���Ă��Ȃ�
	NoIntersection = XMVectorAndInt(
		//�ˉe�x�N�g�������Ȃ烌�C�̕����Ɣ��΁A���C�̌��_�����ɂ���
		XMVectorLess(s, XMVectorZero())
		//l2�̕����傫���Ȃ狅�̊O���ɂ���
		, XMVectorGreater(l2, r2));

	//���̒��S���烌�C�ւ̐��������̔��a�����傫����
	//���C�̍ł��߂��n�_�����̊O���ɂ��邩


	if (XMVector4Greater(m2, r2))
	{
		return false;
	}
	//NoIntersection = XMVectorOrInt(NoIntersection, XMVectorGreater(m2, r2));


	//�Փ˂��Ă���Ƃ��āA�ł��߂��n�_���v�Z

	//���̒��S�ƃ��C�̍ł��߂��_���狅�̕\�ʂ܂ł̋���
	XMVECTOR q = XMVectorSqrt(XMVectorSubtract(r2, m2));

	//r2-m2�����̒l�̏ꍇ��q��0������
	XMVECTOR mask = XMVectorGreater(m2, r2);
	XMVECTOR zero = XMVectorZero();
	//mask��true�̏ꍇ��zero�Afalse�̏ꍇ��q
	q = XMVectorSelect(q, zero, mask);

	//���C�̌��_���狅�Ƃ̌�_�܂ł̋���
	//��O
	XMVECTOR t1 = XMVectorSubtract(s, q);
	//��
	XMVECTOR t2 = XMVectorAdd(s, q);

	//���C�̌��_�����̓�����
	XMVECTOR originInside = XMVectorLessOrEqual(l2, r2);
	
	//��O������true�Ȃ�������Afalse�Ȃ������
	XMVECTOR t = XMVectorSelect(t1, t2, originInside);

	if (XMVector4NotEqualInt(NoIntersection, XMVectorTrueInt()))
	{
		DirectX::XMStoreFloat(dist, t);
		return true;
	}

	return false;
}

bool mtgb::Collider::IsHit(const Vector3& _center, float _radius) const
{
	static Matrix4x4 matrix{};

	if (type == ColliderType::TYPE_SPHERE)
	{
		pTransform_->GenerateWorldMatrix(&matrix);
		Vector3 worldPosition{ Vector3(computeSphere_.Center) * matrix };

		// �����ŋ������

		float distance{ (_center - worldPosition).Size() };
		float hitDistance{ computeSphere_.Radius + _radius };

		// �������o���̋��̔��a������������Γ������Ă���
		return (distance <= hitDistance);
	}
	else if (type == ColliderType::TYPE_CAPSULE)
	{
		// TODO: �J�v�Z���Ƌ��̓����蔻��

	}

	return false;
}

void mtgb::Collider::SetCenter(const Vector3& _center)
{
	if (type == ColliderType::TYPE_AABB)
	{
		computeBox_.Center = _center;
	}
	else
	{
		computeSphere_.Center = _center;
	}
	center = _center;
}

void mtgb::Collider::SetExtents(const Vector3& _extents)
{
	computeBox_.Extents = _extents;
	extents = _extents;
}

void mtgb::Collider::SetRadius(float _radius)
{
	computeSphere_.Radius = _radius;
	radius = _radius;
}

void mtgb::Collider::OnPostRestore()
{
	SetCenter(center);
	switch (type)
	{
	case ColliderType::TYPE_SPHERE:
	case ColliderType::TYPE_CAPSULE:
		SetRadius(radius);
		break;
	case ColliderType::TYPE_AABB:
		SetExtents(extents);
		break;
	}
}

void mtgb::Collider::Draw() const
{
	static Transform copyTransform{};

	//Draw::SetShaderOnce(ShaderType::Debug3D);

	switch (type)
	{
	case ColliderType::TYPE_SPHERE:
		copyTransform = *pTransform_;
		copyTransform.scale *= Vector3::One() * computeSphere_.Radius;
		//copyTransform.position += computeSphere_.Center;
		copyTransform.Compute();
		Draw::FBXModel(hSphereModel_, copyTransform, 0,ShaderType::Debug3D);
		break;
	case ColliderType::TYPE_CAPSULE:
		break;
	case ColliderType::TYPE_AABB:


		if (!isStatic)
		{
			copyTransform = *pTransform_;
		}
		else
		{
			copyTransform.parent = INVALID_ENTITY;
		}
		
		// �����s�Ȃ̂ŉ�]�͂Ȃ�
		copyTransform.rotate = Quaternion{};

		if (isStatic)
		{
			// �ÓI�AStatefulTransform�s�v�Ȃ̂ł��̂܂ܑ��
			copyTransform.position = computeBox_.Center;
			copyTransform.scale = computeBox_.Extents * 2.0f;
		}
		else
		{
			// StatefulTransform�ɍ��킹�Ĉʒu�A�T�C�Y�𒲐�
			//copyTransform.position += computeBox_.Center;
			copyTransform.scale *= computeBox_.Extents * 2.0f;
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


