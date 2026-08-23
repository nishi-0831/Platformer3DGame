#pragma once
#include <DirectXMath.h>

namespace mtgb
{
	struct Vector2 : DirectX::XMFLOAT2
	{
		Vector2(float _x, float _y);
		Vector2()
			: Vector2 { 0, 0 }
		{
		}

		static inline Vector2 Zero()
		{
			return Vector2 { 0, 0 };
		}
		static inline Vector2 One()
		{
			return Vector2 { 1, 1 };
		}
	};
} // namespace mtgb
