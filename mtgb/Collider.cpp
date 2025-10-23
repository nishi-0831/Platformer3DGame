#include "Collider.h"
#include "ColliderMemento.h"
#include "Transform.h"
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
	: IComponent{ _entityId }
	, pTransform_{&Transform::Get(_entityId)}
	, isStatic_{false}
	, colliderTag_{ColliderTag::GAME_OBJECT}
{
}



mtgb::Collider::Collider(EntityId _entityId, ColliderTag _colliderTag)
	: IComponent{ _entityId }
	, colliderTag_{_colliderTag}
{

	switch (_colliderTag)
	{
		// ���݂̓Q�[���I�u�W�F�N�g�͓��I�A�X�e�[�W�͐ÓI�ƒf�肵�Ă��邪
		// ���I�ȃX�e�[�W�Ȃǂ��ǉ�����邩������Ȃ��̂Œ���
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
	switch (type_)
	{
	case TYPE_SPHERE:
		UpdateBoundingSphere();
		break;
	case TYPE_AABB:
		UpdateBoundingBox();
		break;
	case TYPE_CAPSULE:
		// TODO: �J�v�Z��������
		break;
	}
}

void mtgb::Collider::UpdateBoundingSphere()
{
	// Transform���猻�݂̈ʒu���擾����BoundingSphere���X�V
	
	//pTransform_->GenerateWorldMatrix(&matrix);
		
	//computeSphere_.Center = pTransform_->position + computeSphere_.Center;
	//computeSphere_.Center = Vector3(computeSphere_.Center) * matrix;
	computeSphere_.Center = pTransform_->position;
}

void mtgb::Collider::UpdateBoundingBox()
{
	if (!isStatic_)
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
	if (colliderTag_ == ColliderTag::STAGE && _other.colliderTag_ == ColliderTag::STAGE)
	{
		return false;
	}

	if (type_ == _other.type_)
	{
		if (type_ == TYPE_SPHERE)
		{
			//pTransform_->GenerateWorldMatrix(&matrix);
			//Vector3 worldPosition{ Vector3(computeSphere_.Center) * matrix };

			//_other.pTransform_->GenerateWorldMatrix(&matrix);
			//Vector3 otherWorldPosition{ (_other.computeSphere_.Center) * matrix };

			//float distance{ (otherWorldPosition - worldPosition).Size() };
			//float hitDistance{ computeSphere_.Radius + _other.computeSphere_.Radius};

			//// �������o���̋��̔��a������������Γ������Ă���
			//return (distance <= hitDistance);
			return computeSphere_.Intersects(_other.computeSphere_);
		}
		else if (type_ == TYPE_AABB)
		{
			return computeBox_.Intersects(_other.computeBox_);
		}
	}
	else
	{
		if (type_ == TYPE_SPHERE)
		{	
			return computeSphere_.Intersects(_other.computeBox_);
		}
		else if (type_ == TYPE_AABB)
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

	if (type_ == TYPE_SPHERE)
	{
		pTransform_->GenerateWorldMatrix(&matrix);
		Vector3 worldPosition{ Vector3(computeSphere_.Center) * matrix };

		// �����ŋ������

		float distance{ (_center - worldPosition).Size() };
		float hitDistance{ computeSphere_.Radius + _radius };

		// �������o���̋��̔��a������������Γ������Ă���
		return (distance <= hitDistance);
	}
	else if (type_ == TYPE_CAPSULE)
	{
		// TODO: �J�v�Z���Ƌ��̓����蔻��

	}

	return false;
}

void mtgb::Collider::SetCenter(const Vector3& _center)
{
	if (type_ == TYPE_AABB)
	{
		computeBox_.Center = _center;
	}
	else
	{
		computeSphere_.Center = _center;
	}
}

void mtgb::Collider::SetExtents(const Vector3& _extents)
{
	computeBox_.Extents = _extents;
}

void mtgb::Collider::SetRadius(float _radius)
{
	computeSphere_.Radius = _radius;
}

void mtgb::Collider::Draw() const
{
	static Transform copyTransform{};

	//Draw::SetShaderOnce(ShaderType::Debug3D);

	switch (type_)
	{
	case mtgb::ColliderType::TYPE_SPHERE:
		copyTransform = *pTransform_;
		copyTransform.scale *= Vector3::One() * computeSphere_.Radius;
		//copyTransform.position += computeSphere_.Center;
		copyTransform.Compute();
		Draw::FBXModel(hSphereModel_, copyTransform, 0,ShaderType::Debug3D);
		break;
	case mtgb::ColliderType::TYPE_CAPSULE:
		break;
	case mtgb::ColliderType::TYPE_AABB:


		if (!isStatic_)
		{
			copyTransform = *pTransform_;
		}
		else
		{
			copyTransform.parent = INVALID_ENTITY;
		}
		
		// �����s�Ȃ̂ŉ�]�͂Ȃ�
		copyTransform.rotate = Quaternion{};

		if (isStatic_)
		{
			// �ÓI�Atransform�s�v�Ȃ̂ł��̂܂ܑ��
			copyTransform.position = computeBox_.Center;
			copyTransform.scale = computeBox_.Extents * 2.0f;
		}
		else
		{
			// transform�ɍ��킹�Ĉʒu�A�T�C�Y�𒲐�
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

void mtgb::Collider::RestoreFromMemento(const ColliderMemento& _memento)
{
	this->type_ = _memento.type_;
	this->isStatic_ = _memento.isStatic_;
	this->colliderTag_ = _memento.colliderTag_;
}
