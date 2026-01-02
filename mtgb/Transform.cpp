#include "Transform.h"

#include "MTImGui.h"
namespace
{
	Matrix4x4 m;
}

mtgb::Transform::Transform()
	: parent(INVALID_ENTITY)
	, position{Vector3::Zero()}
	, rotate{Quaternion::Identity()}
	, scale{Vector3::One()}
	, localPosition_{Vector3::Zero()}
	, localRotate_{Quaternion::Identity()}
	, localScale_{Vector3::One()}
{
}

mtgb::Transform::Transform(EntityId _entityId)
	: IComponent(_entityId)
	, parent(INVALID_ENTITY)
	, position{Vector3::Zero()}
	, rotate{Quaternion::Identity()}
	, scale{Vector3::One()}
	, localPosition_{Vector3::Zero()}
	, localRotate_{Quaternion::Identity()}
	, localScale_{Vector3::One()}
{
}

mtgb::Transform::~Transform()
{
}

mtgb::Transform& mtgb::Transform::operator=(const Transform& _other)
{
	if (&_other == this)
	{
		return *this;
	}

	this->parent   = _other.parent;
	this->position = _other.position;
	this->scale	   = _other.scale;
	this->rotate   = _other.rotate;

	this->matrixTranslate_	= _other.matrixTranslate_;
	this->matrixRotate_		= _other.matrixRotate_;
	this->matrixScale_		= _other.matrixScale_;
	this->matrixWorld_		= _other.matrixWorld_;
	this->matrixWorldRot_	= _other.matrixWorldRot_;
	this->matrixLocal_		= _other.matrixLocal_;
	this->prevParentMatrix_ = _other.prevParentMatrix_;
	this->localPosition_	= _other.localPosition_;
	this->localRotate_		= _other.localRotate_;
	this->localScale_		= _other.localScale_;

	return *this;
}

void mtgb::Transform::Compute()
{
	using DirectX::XMConvertToRadians;
	using DirectX::XMMatrixRotationQuaternion;
	using DirectX::XMMatrixScaling;
	using DirectX::XMMatrixTranslation;
	using DirectX::XMQuaternionNormalize;

	// 親の行列を取得
	Matrix4x4 parentMat{};
	GenerateParentMatrix(&parentMat);

	// 現在のワールド座標から行列を作成
	Matrix4x4 currWorldMat = XMMatrixScaling(scale.x, scale.y, scale.z) *
							 XMMatrixRotationQuaternion(XMQuaternionNormalize(rotate)) *
							 XMMatrixTranslation(position.x, position.y, position.z);
	Matrix4x4 parentInverse = DirectX::XMMatrixInverse(nullptr, prevParentMatrix_);

	matrixLocal_ = currWorldMat * parentInverse;
	DecomposeMatrixImpl(&localPosition_, &localRotate_, &localScale_, matrixLocal_);

	GenerateWorldMatrix(&matrixWorld_);
	// ワールド行列を計算
	GenerateWorldRotationMatrix(&matrixWorldRot_); // ワールド回転行列更新

	// ワールド行列からワールド座標系の値を逆算して更新
	DecomposeMatrixImpl(&position, &rotate, &scale, matrixWorld_);

	/*matrixTranslate_ = XMMatrixTranslation(position.x, position.y, position.z);
	matrixRotate_ = XMMatrixRotationQuaternion(
		XMQuaternionNormalize(rotate));
	matrixScale_ = XMMatrixScaling(scale.x, scale.y, scale.z);*/
	prevParentMatrix_ = parentMat;
}

void mtgb::Transform::GenerateLocalMatrix(Matrix4x4* _pMatrix) const
{
	*_pMatrix *= matrixScale_;
	*_pMatrix *= matrixRotate_;
	*_pMatrix *= matrixTranslate_;
}

void mtgb::Transform::GenerateWorldMatrix(Matrix4x4* _pMatrix) const
{
	*_pMatrix = DirectX::XMMatrixIdentity();
	GenerateWorldMatrixSelf(_pMatrix);
}

void mtgb::Transform::GenerateWorldRotationMatrix(Matrix4x4* _pMatrix) const
{
	*_pMatrix = DirectX::XMMatrixIdentity();
	GenerateWorldRotMatrixSelf(_pMatrix);
}

void mtgb::Transform::GenerateParentMatrix(Matrix4x4* _pMatrix) const
{
	Transform* parent = GetParent();
	if (parent == nullptr)
		return;

	parent->GenerateWorldMatrixSelf(_pMatrix);
}

void mtgb::Transform::GenerateParentRotationMatrix(Matrix4x4* _pMatrix) const
{
	*_pMatrix *= matrixRotate_;
}

bool mtgb::Transform::DecomposeMatrix(const Matrix4x4& _matrix)
{
	return DecomposeMatrixImpl(&position, &rotate, &scale, _matrix);
}

bool mtgb::Transform::DecomposeMatrixImpl(Vector3* _pPos, Quaternion* _pRot, Vector3* _pScale, const Matrix4x4& _matrix)
{
	DirectX::XMVECTOR vPos	 = DirectX::XMLoadFloat3(_pPos);
	DirectX::XMVECTOR vScale = DirectX::XMLoadFloat3(_pScale);

	bool result = DirectX::XMMatrixDecompose(&vScale, &_pRot->v, &vPos, _matrix);

	DirectX::XMStoreFloat3(_pPos, vPos);
	DirectX::XMStoreFloat3(_pScale, vScale);
	return result;
}

mtgb::Transform* mtgb::Transform::GetParent() const
{
	// 親Idが無効値なら nullptr を返す
	if (parent == INVALID_ENTITY)
	{
		return nullptr;
	}
	else
	{
		return &Transform::Get(parent);
	}
}

void mtgb::Transform::SetParent(const EntityId _entityId)
{
	if (_entityId == GetEntityId() || _entityId == parent)
		return;

	Transform& parentTransform = Get(_entityId);

	if (parent == INVALID_ENTITY)
	{
		Matrix4x4 parentInverse = DirectX::XMMatrixInverse(nullptr, parentTransform.matrixWorld_);
		Matrix4x4 localMatrix	= matrixWorld_ * parentInverse;
		DecomposeMatrixImpl(&localPosition_, &localRotate_, &localScale_, localMatrix);

		parent = _entityId;

		GenerateParentMatrix(&prevParentMatrix_);
	}
	else if (_entityId == INVALID_ENTITY)
	{
		matrixLocal_	  = matrixWorld_;
		parent			  = _entityId;
		prevParentMatrix_ = DirectX::XMMatrixIdentity();
	}
	Compute();
}

void mtgb::Transform::Rotation(const Vector3& _rotate)
{
	using DirectX::XMQuaternionMultiply;
	using DirectX::XMQuaternionNormalize;
	using DirectX::XMQuaternionRotationRollPitchYaw;

	// rotate
	rotate = XMQuaternionMultiply(rotate, XMQuaternionRotationRollPitchYaw(_rotate.x, _rotate.y, _rotate.z));
}

void mtgb::Transform::Rotation(const Vector3& _axis, const float _angle)
{
	using DirectX::XMQuaternionRotationAxis;

	rotate *= XMQuaternionRotationAxis(_axis, _angle);
}

mtgb::Vector3 mtgb::Transform::Forward() const
{
	return Vector3::Forward() * matrixWorldRot_;
}

mtgb::Quaternion mtgb::Transform::GetWorldRotate() const
{
	using DirectX::XMQuaternionIdentity;
	using DirectX::XMQuaternionMultiply;
	using DirectX::XMQuaternionRotationMatrix;

	return XMQuaternionMultiply(XMQuaternionRotationMatrix(matrixWorldRot_), XMQuaternionIdentity());
}

void mtgb::Transform::OnPostRestore()
{
	Compute();
}

void mtgb::Transform::GenerateWorldMatrixSelf(Matrix4x4* _pMatrix) const
{
	if (parent != INVALID_ENTITY)
	{
		GetParent()->GenerateWorldMatrixSelf(_pMatrix);
		*_pMatrix = matrixLocal_ * (*_pMatrix);
	}
	else
	{
		*_pMatrix = matrixLocal_;
	}
}

void mtgb::Transform::GenerateWorldRotMatrixSelf(Matrix4x4* _pMatrix) const
{
	Matrix4x4 matLocalRot = DirectX::XMMatrixRotationQuaternion(XMQuaternionNormalize(localRotate_));
	if (parent != INVALID_ENTITY)
	{
		Matrix4x4 mWorldRotParent{};
		GetParent()->GenerateWorldRotMatrixSelf(&mWorldRotParent);
		*_pMatrix = matLocalRot * mWorldRotParent;
	}
	else
	{
		*_pMatrix = matLocalRot;
	}
}