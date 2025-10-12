#pragma once
#include <DirectXMath.h>
#include <cmath>

namespace mtgb
{
	struct Vector3 : DirectX::XMFLOAT3
	{
		Vector3(
			const float _x,
			const float _y,
			const float _z);
		Vector3() :
			Vector3{ 0, 0, 0 }
		{}
		Vector3(const Vector3& _other) :
			Vector3{_other.x,_other.y,_other.z}
		{}
		Vector3(const DirectX::XMFLOAT3& _other) :
			Vector3{ _other.x,_other.y,_other.z }
		{}
		Vector3(const DirectX::XMVECTOR& _v) :
			Vector3{ _v.m128_f32[0], _v.m128_f32[1], _v.m128_f32[2] }
		{}

		inline operator DirectX::XMVECTOR() const { return DirectX::XMLoadFloat3(this); }

		//inline Vector3& operator=(const Vector3& _other) { x = _other.x; y = _other.y; z = _other.z; return *this; }
		//inline Vector3& operator=(const DirectX::XMFLOAT3& _other) { x = _other.x; y = _other.y; z = _other.z; return *this; }

		inline Vector3& operator*=(const DirectX::XMMATRIX _matrix)
		{
			using DirectX::XMVECTOR;
			using DirectX::XMVector3TransformCoord;
			using DirectX::XMStoreFloat3;

			XMVECTOR v{ (XMVECTOR)*this };
			v = XMVector3TransformCoord(v, _matrix);
			XMStoreFloat3(this, v);

			return *this;
		}
		inline Vector3& operator*=(const float _value) { x *= _value; y *= _value; z *= _value;  return *this; }
		inline Vector3& operator*=(const Vector3& _other) { x *= _other.x; y *= _other.y; z *= _other.z; return *this; }
		inline Vector3& operator/=(const Vector3& _other) { x /= _other.y; y /= _other.y; z /= _other.z; return *this; }
		inline Vector3& operator/=(const float _value) { x /= _value; y /= _value; z /= _value;  return *this; }
		inline Vector3& operator%=(const float _value) { x = std::fmodf(x, _value); y = std::fmodf(y, _value); z = std::fmodf(z, _value);  return *this; }
		inline Vector3& operator+=(const Vector3& _other) { x += _other.x; y += _other.y; z += _other.z; return *this; }
		inline Vector3& operator-=(const Vector3& _other) { x -= _other.x; y -= _other.y; z -= _other.z; return *this; }

		inline float Size() const { return std::sqrtf(x * x + y * y + z * z); };

		Vector3& Normalize() { *this /= Size(); return *this; }
		static Vector3 Normalize(const Vector3& _v) { return Vector3{ _v } /= _v.Size(); }

		/// <summary>
		/// ��x�N�g��
		/// </summary>
		static inline auto Zero()    { return Vector3{  0,  0,  0 }; }
		/// <summary>
		/// �P�ʃx�N�g��
		/// </summary>
		static inline auto One()     { return Vector3{  1,  1,  1 }; }
		static inline auto Up()      { return Vector3{  0,  1,  0 }; }
		static inline auto Down()    { return Vector3{  0, -1,  0 }; }
		static inline auto Left()    { return Vector3{ -1,  0,  0 }; }
		static inline auto Right()   { return Vector3{  1,  0,  0 }; }
		static inline auto Back()    { return Vector3{  0,  0, -1 }; }
		static inline auto Forward() { return Vector3{  0,  0,  1 }; }
	};

	inline Vector3 operator*(const Vector3& _v, const DirectX::XMMATRIX _matrix) { return Vector3{ _v } *= _matrix; }
	inline Vector3 operator*(const Vector3& _v, const float _value) { return Vector3{ _v } *= _value; }
	inline Vector3 operator*(const float _value, const Vector3& _v) { return Vector3{ _v } *= _value; }
	inline Vector3 operator/(const Vector3& _v, const float _value) { return Vector3{ _v } /= _value; }
	inline Vector3 operator/(const float _value, const Vector3& _v) { return Vector3{ _v } /= _value; }
	inline Vector3 operator%(const Vector3& _v, const float _value) { return Vector3{ _v } %= _value; }
	inline Vector3 operator%(const float _value, const Vector3& _v) { return Vector3{ _v } %= _value; }
	inline Vector3 operator+(const Vector3& _v1, const Vector3& _v2) { return Vector3{ _v1 } += _v2; }
	inline Vector3 operator-(const Vector3& _v1, const Vector3& _v2) { return Vector3{ _v1 } -= _v2; }
}
