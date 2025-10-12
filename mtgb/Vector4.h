#pragma once
#include <DirectXMath.h>
#include "Vector3.h"

namespace mtgb
{
	/// <summary>
	/// 4�����x�N�g��
	/// </summary>
	struct Vector4 : DirectX::XMVECTORF32
	{
		/// <summary>
		/// 4�����x�N�g��
		/// </summary>
		/// <param name="_x">x����</param>
		/// <param name="_y">y����</param>
		/// <param name="_z">z����</param>
		/// <param name="_w">w����</param>
		Vector4(
			const float _x,
			const float _y,
			const float _z,
			const float _w);
		/// <summary>
		/// Vector3����ÖٓI�ȕϊ�
		/// </summary>
		/// <param name="_vec3">�ϊ���</param>
		Vector4(const Vector3& _vec3);

		Vector4() :
			Vector4{ 0.0f, 0.0f, 0.0f, 0.0f }
		{
		}

		static inline auto Zero() { return Vector4{ 0,  0,  0,  0 }; }
		static inline auto One()  { return Vector4{ 1,  1,  1,  1 }; }

		inline Vector4& operator+=(const Vector3& _other) { f[0] += _other.x; f[1] += _other.y; f[2] += _other.z; return *this; }
		inline Vector4& operator-=(const Vector3& _other) { f[0] -= _other.x; f[1] -= _other.y; f[2] -= _other.z; return *this; }
	};
}
