#pragma once
#include "Vector2T.h"


namespace mtgb
{
	using Vector2Int = Vector2T<int>;
	/*template<>
	inline Vector2T<int32_t> Vector2T<int32_t>::Zero()
	{
		return Vector2T<int32_t>(0, 0);
	}
	template<>
	inline Vector2T<int32_t> Vector2T<int32_t>::One()
	{
		return Vector2T<int32_t>(1, 1);
	}*/

//	//struct Vector2;
//	
//	/// <summary>
//	/// 2�����̐����^�x�N�g��
//	/// </summary>
//	struct Vector2Int :  Vector2T<int32_t>
//	{
//		//int x;  // x����
//		//int y;  // y����
//	
//		Vector2Int() :
//			Vector2Int{ 0, 0 }
//		{}
//	
//		Vector2Int(const int32_t& x, const int32_t& y) :
//			Vector2T<int32_t>{ x, y }
//		{}
//	
//#pragma region �l�����Z
//		inline Vector2Int& operator*=(const int& _scalar) { x *= _scalar; y *= _scalar; return *this; }
//		inline Vector2Int& operator/=(const int& _scalar) { x /= _scalar; y /= _scalar; return *this; }
//		inline Vector2Int& operator+=(const Vector2Int& _other) { x += _other.x; y += _other.y; return *this; }
//		inline Vector2Int& operator-=(const Vector2Int& _other) { x -= _other.x; y -= _other.y; return *this; }
//#pragma endregion
//	
//#pragma region �L���X�g
//		//explicit operator Vector2() const noexcept;
//#pragma endregion
//	
//#pragma region �萔
//		inline static Vector2Int Zero() { return { 0, 0 }; }
//		inline static Vector2Int One() { return { 1, 1 }; }
//#pragma endregion
//	};
//
//#pragma region �O���[�o���I�y���[�^
//	inline Vector2Int operator*(const Vector2Int& _v, const int& _scalar) { return Vector2Int{ _v } *= _scalar; }
//	inline Vector2Int operator*(const int& _scalar, const Vector2Int& _v) { return Vector2Int{ _v } *= _scalar; }
//	inline Vector2Int operator/(const Vector2Int& _v, const int& _scalar) { return Vector2Int{ _v } /= _scalar; }
//	inline Vector2Int operator/(const int& _scalar, const Vector2Int& _v) { return Vector2Int{ _v } /= _scalar; }
//	inline Vector2Int operator+(const Vector2Int& _v1, const Vector2Int& _v2) { return Vector2Int{ _v1 } += _v2; }
//	inline Vector2Int operator-(const Vector2Int& _v1, const Vector2Int& _v2) { return Vector2Int{ _v1 } -= _v2; }
//	
//	inline bool operator==(const Vector2Int& _v1, const Vector2Int& _v2) { return _v1.x - _v2.x == 0 && _v1.y - _v2.y == 0; }
//	inline bool operator!=(const Vector2Int& _v1, const Vector2Int& _v2) { return !(_v1 == _v2); }
//#pragma endregion
}
