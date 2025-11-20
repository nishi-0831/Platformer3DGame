#pragma once
#include <DirectXMath.h>
#include <string>
#include <cmath>

namespace mtgb
{
	struct Matrix4x4 : DirectX::XMMATRIX
	{
		static const size_t ROW_SIZE{ 4 };
		static const size_t COLUMN_SIZE{ 4 };

		//using DirectX::XMMATRIX::XMMATRIX;

		inline Matrix4x4() :
			XMMATRIX{ DirectX::XMMatrixIdentity() }
		{}

		inline Matrix4x4(const Matrix4x4& _other) = default;

		inline Matrix4x4(const DirectX::XMMATRIX& _xmMatrix) :
			XMMATRIX{ _xmMatrix }
		{}

		inline Matrix4x4& operator=(const Matrix4x4& _other)
		{
			if (this != &_other)
			{
				XMMATRIX::operator=(_other);
			}
			return *this;
		}
		
		inline bool Equals(const Matrix4x4& _other) const
		{
			//FLT_EPSILON
			DirectX::XMFLOAT4X4 a{};
			DirectX::XMFLOAT4X4 b{};
			DirectX::XMStoreFloat4x4(&a, *this);
			DirectX::XMStoreFloat4x4(&b, _other);

			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					if (std::abs(a.m[i][j] - b.m[i][j] > FLT_EPSILON))
					{
						return false;
					}
				}
			}
			return false;
		}

		inline bool operator==(const Matrix4x4& _other) const
		{
			return Equals(_other);
		}

		inline bool operator!=(const Matrix4x4& _other) const
		{
			return !(*this == _other);
		}
		std::string ToString();
	};
}
