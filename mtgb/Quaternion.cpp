#include "Quaternion.h"
#include "Matrix4x4.h"
#include "Vector4.h"


mtgb::Quaternion mtgb::Quaternion::LookRotation(const Vector3& _dir, const Vector3& _upVec)
{
	using DirectX::XMVector3Normalize;
	using DirectX::XMVector3Cross;
	using DirectX::XMQuaternionRotationMatrix;

	Vector3 forward{ XMVector3Normalize(_dir) };
	Vector3 up{ XMVector3Normalize(_upVec) };

	// �O�ςŉE�x�N�g�������߂�
	Vector3 right{ XMVector3Normalize(XMVector3Cross(up, forward)) };

	// �����ȏ�x�N�g�����Čv�Z
	Vector3 verticalUp{ XMVector3Cross(forward, right) };

	// ��]�s��ɂ���
	Matrix4x4 mRot{ DirectX::XMMATRIX{ right, verticalUp, forward, Vector4{ 0, 0, 0, 1 } } };

	return XMQuaternionRotationMatrix(mRot);
}

std::string mtgb::Quaternion::ToString() const
{
	std::string str{ "(" };
	for (int i = 0; i < Quaternion::COUNT; i++)
	{
		str += std::to_string(f[i]);
		if (i < Quaternion::COUNT - 1)
		{
			str += ", ";
		}
	}
	str += ")";

	return str;
}
