#pragma once
#include <DirectXMath.h>
#include <string>
#include "Vector3.h"


namespace mtgb
{
	/// <summary>
	/// �l�����̍\����
	/// </summary>
	struct Quaternion : DirectX::XMVECTORF32
	{
		static const size_t COUNT{ 4 };

		inline const float& X() const { return f[0]; }
		inline const float& Y() const { return f[1]; }
		inline const float& Z() const { return f[2]; }
		inline const float& W() const { return f[3]; }
		inline float& X() { return f[0]; }
		inline float& Y() { return f[1]; }
		inline float& Z() { return f[2]; }
		inline float& W() { return f[3]; }

		Quaternion() :
			Quaternion{ 0, 0, 0, 0 }
		{}
		Quaternion(float _x, float _y, float _z, float _w)
		{
			X() = _x;
			Y() = _y;
			Z() = _z;
			W() = _w;
		}
		Quaternion(const DirectX::XMVECTORF32& _v)
		{
			v = _v;
		}
		Quaternion(const DirectX::XMVECTOR& _v)
		{
			v = _v;
		}

		/// <summary>
		/// �P�ʎl�������擾
		/// </summary>
		/// <returns>�P��</returns>
		static Quaternion Identity() { return DirectX::XMQuaternionIdentity(); }

		//static 

		/// <summary>
		/// �I�C���[�p���l�����ɕϊ�
		/// </summary>
		/// <param name="_vec">�I�C���[�p�̃x�N�g��</param>
		/// <returns>�l����</returns>
		static Quaternion Euler(const Vector3& _vec) { return DirectX::XMQuaternionRotationRollPitchYaw(_vec.x, _vec.y, _vec.z); }
		/// <summary>
		/// �t�l����/�������擾
		/// </summary>
		/// <returns>�t�l����</returns>
		Quaternion GetInverse() const { return { -X(), -Y(), -Z(), W() }; }
		/// <summary>
		/// �t�l����/�������擾
		/// </summary>
		/// <param name="_q">�擾����l����</param>
		/// <returns>�t�l����</returns>
		static Quaternion GetInverse(const Quaternion& _q) { return _q.GetInverse(); }
		/// <summary>
		/// �l�����̃T�C�Y���擾����
		/// </summary>
		/// <returns>�l�����̃T�C�Y</returns>
		float GetSize() const { return X() * X() + Y() * Y() + Z() * Z() + W() * W(); }
		/// <summary>
		/// �l�����̃T�C�Y���擾����
		/// </summary>
		/// <param name="_q">�擾����l����</param>
		/// <returns>�l�����̃T�C�Y</returns>
		static float GetSize(const Quaternion& _q) { return _q.GetSize(); }

		static Quaternion SLerp(const Quaternion& _self, const Quaternion& _to, float _lerp)
		{
			return DirectX::XMQuaternionSlerp(_self, _to, _lerp);
		}

		/// <summary>
		/// ��������ւ̉�]���擾����
		/// </summary>
		/// <param name="_dir">�����x�N�g��</param>
		/// <param name="_upVec">������̃x�N�g��</param>
		/// <returns>��]�̎l����</returns>
		static Quaternion LookRotation(const Vector3& _dir, const Vector3& _upVec);

		/// <summary>
		/// �������Ă�������ƁA���ꂩ�����������������l�������쐬
		/// </summary>
		/// <param name="_fromDir">�������Ă�������x�N�g��</param>
		/// <param name="_toDir">�������������x�N�g��</param>
		/// <returns>�l����</returns>
		static Quaternion FromToRotation(const Vector3& _fromDir, const Vector3& _toDir)
		{
			// MEMO: 1. ���K��(v1, v2)
			// MEMO: 2. �O�ςŉ�]���̐��K��(v = (x, y, z))�Q�b�g norm(Vf x Vt)
			// MEMO: 3. ���ςŉ�]�p�x(th)�Q�b�g acos(Vf �E Vt)
			// MEMO: 4. �l�������� 
			// MEMO: -. q = (x sin(th / 2), y sin(th / 2), z sin(th / 2), cos(th / 2))

			Vector3 v1{ Vector3::Normalize(_fromDir) };
			Vector3 v2{ Vector3::Normalize(_toDir) };

			Vector3 axis{ DirectX::XMVector3Cross(v1, v2) };
			float th{ acosf(DirectX::XMVector3Dot(v1, v2).m128_f32[0]) };
			float s{ sinf(th * 0.5f) };
			float w{ cosf(th * 0.5f) };

			return { axis.x * s, axis.y * s, axis.z * s, w };
		}

		/// <summary>
		/// ������ɕϊ�����
		/// </summary>
		/// <returns>������</returns>
		std::string ToString() const;

		inline Quaternion& operator*= (const Quaternion& _other) { *this = DirectX::XMQuaternionMultiply(*this, _other); return *this; };
		//inline Quaternion& operator+=(const Quaternion& _other) { f[0] += f[0]; f[1] += f[1]; f[2] += f[2]; f[3] += f[3]; return *this; }
		//inline Quaternion& operator-=(const Quaternion& _other) { f[0] -= f[0]; f[1] -= f[1]; f[2] -= f[2]; f[3] -= f[3]; return *this; }
	};

	inline Quaternion operator* (const Quaternion& _q1, const Quaternion& _q2) { return Quaternion{ _q1 } *= _q2; };
}
