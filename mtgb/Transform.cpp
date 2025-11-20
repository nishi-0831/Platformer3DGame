#include "Transform.h"

mtgb::Transform::~Transform()
{

}



void mtgb::Transform::Compute()
{
	using DirectX::XMMatrixTranslation;
	using DirectX::XMMatrixRotationQuaternion;
	using DirectX::XMQuaternionNormalize;
	using DirectX::XMConvertToRadians;
	using DirectX::XMMatrixScaling;

	// 親の行列を取得
	Matrix4x4 parentMat;
	GenerateParentMatrix(&parentMat);

	// 現在のワールド座標から行列を作成
	Matrix4x4 currWorldMat =
		XMMatrixScaling(scale.x, scale.y, scale.z) *
		XMMatrixRotationQuaternion(XMQuaternionNormalize(rotate)) *
		XMMatrixTranslation(position.x, position.y, position.z);

	// ローカル行列を計算
	Matrix4x4 parentInverse = DirectX::XMMatrixInverse(nullptr, parentMat);
	matrixLocal_ = currWorldMat * parentInverse;

	// ローカル座標系の値を更新
	DecomposeMatrixImpl(&localPosition_, &localRotate_, &localScale_, matrixLocal_);

	// ローカル変換行列
	matrixTranslate_ = XMMatrixTranslation(localPosition_.x, localPosition_.y, localPosition_.z);
	matrixRotate_ = XMMatrixRotationQuaternion(
		XMQuaternionNormalize(localRotate_));
	matrixScale_ = XMMatrixScaling(localScale_.x, localScale_.y, localScale_.z);

	// ワールド行列を再計算
	matrixWorld_ = matrixLocal_ * parentMat;
	//GenerateWorldMatrix(&matrixWorld_);             // ワールド行列更
	GenerateWorldRotationMatrix(&matrixWorldRot_);  // ワールド回転行列更新
	
	if (parent != INVALID_ENTITY)
	{
		DecomposeMatrix(matrixWorld_);
	}
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
	DirectX::XMVECTOR vPos = DirectX::XMLoadFloat3(_pPos);
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

	if (_entityId == INVALID_ENTITY)
	{

	}
	if (parent == INVALID_ENTITY)
	{ 
		Transform& parentTransform = Get(_entityId);
		Matrix4x4 parentInverse = DirectX::XMMatrixInverse(nullptr, parentTransform.matrixWorld_);
		Matrix4x4 localMatrix = matrixWorld_ * parentInverse;
		DecomposeMatrixImpl(&localPosition_, &localRotate_, &localScale_, localMatrix);
		
		parent = _entityId;
	}
}

void mtgb::Transform::Rotation(const Vector3& _rotate)
{
	using DirectX::XMQuaternionMultiply;
	using DirectX::XMQuaternionRotationRollPitchYaw;
	using DirectX::XMQuaternionNormalize;

	//rotate
	rotate = XMQuaternionMultiply(
		rotate,
		XMQuaternionRotationRollPitchYaw(_rotate.x, _rotate.y, _rotate.z));
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
	using DirectX::XMQuaternionRotationMatrix;
	using DirectX::XMQuaternionMultiply;
	using DirectX::XMQuaternionIdentity;

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
		*_pMatrix = matrixScale_ * matrixRotate_ * matrixTranslate_ * (*_pMatrix);
	}
	else
	{
		*_pMatrix = matrixScale_ * matrixRotate_ * matrixTranslate_;
	}
}

void mtgb::Transform::GenerateWorldRotMatrixSelf(Matrix4x4* _pMatrix) const
{
	if (parent != INVALID_ENTITY)
	{
		Matrix4x4 mWorldRotParent{};
		GetParent()->GenerateWorldRotMatrixSelf(&mWorldRotParent);
		*_pMatrix = matrixRotate_ * mWorldRotParent;
	}
	else
	{
		*_pMatrix = matrixRotate_;
	}
}