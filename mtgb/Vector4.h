#pragma once
#include <DirectXMath.h>
#include "Vector3.h"

namespace mtgb
{
	/// <summary>
	/// 4次元ベクトル
	/// </summary>
	struct Vector4 : DirectX::XMVECTORF32
	{
		/// <summary>
		/// 4次元ベクトル
		/// </summary>
		/// <param name="_x">x成分</param>
		/// <param name="_y">y成分</param>
		/// <param name="_z">z成分</param>
		/// <param name="_w">w成分</param>
		Vector4(float _x, float _y, float _z, float _w);
		/// <summary>
		/// Vector3から明示的な変換
		/// </summary>
		/// <param name="_vec3">変換元</param>
		explicit Vector4(const Vector3& _vec3);

		Vector4()
			: Vector4 { 0.0f, 0.0f, 0.0f, 0.0f }
		{
		}

		static inline Vector4 Zero()
		{
			return Vector4 { 0, 0, 0, 0 };
		}
		static inline Vector4 One()
		{
			return Vector4 { 1, 1, 1, 1 };
		}

		inline Vector4& operator+=(const Vector3& _other)
		{
			f[0] += _other.x;
			f[1] += _other.y;
			f[2] += _other.z;
			return *this;
		}
		inline Vector4& operator-=(const Vector3& _other)
		{
			f[0] -= _other.x;
			f[1] -= _other.y;
			f[2] -= _other.z;
			return *this;
		}
	};
	inline Vector4 operator+(const Vector4& _lhs, const Vector4& _rhs)
	{
		return Vector4 { _lhs.f[0] + _rhs.f[0], _lhs.f[1] + _rhs.f[1], _lhs.f[2] + _rhs.f[2], _lhs.f[3] + _rhs.f[3] };
	}
} // namespace mtgb
