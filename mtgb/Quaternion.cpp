#include "Quaternion.h"
#include "Matrix4x4.h"
#include "Vector4.h"

mtgb::Quaternion mtgb::Quaternion::LookRotation(const Vector3& _dir, const Vector3& _upVec)
{
	using DirectX::XMQuaternionRotationMatrix;
	using DirectX::XMVector3Cross;
	using DirectX::XMVector3Normalize;

	Vector3 forward { XMVector3Normalize(_dir) };
	Vector3 up { XMVector3Normalize(_upVec) };

	// 外積で右ベクトルを求める
	Vector3 right { XMVector3Normalize(XMVector3Cross(up, forward)) };

	// 垂直な上ベクトルを再計算
	Vector3 verticalUp { XMVector3Cross(forward, right) };

	// 回転行列にする
	Matrix4x4 mRot { DirectX::XMMATRIX { right, verticalUp, forward, Vector4 { 0, 0, 0, 1 } } };

	return XMQuaternionRotationMatrix(mRot);
}
