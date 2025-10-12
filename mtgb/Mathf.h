#pragma once
#include <cmath>
#include "MTAssert.h"
#include "Vector3.h"
#include "GameTime.h"

namespace mtgb::Mathf
{
	/// <summary>
	/// a����b����`��Ԃ���
	/// </summary>
	/// <param name="_a">a�l</param>
	/// <param name="_b">b�l</param>
	/// <param name="_value">0.0f ~ 1.0f</param>
	/// <returns>�ŏ��l����ő�l����`��Ԃ���_value�̒l</returns>
	static inline float LerpForward(const float _a, const float _b, const float _value)
	{
		float min{ std::fminf(_a, _b) };
		float max{ std::fmaxf(_a, _b) };
		float rate{ std::fmaxf(0.0f, std::fminf(_value, 1.0f)) };
		
		return min + (max - min) * rate;
	}

	/// <summary>
	/// a����b����`��Ԃ���
	/// </summary>
	/// <param name="_a">a�l</param>
	/// <param name="_b">b�l</param>
	/// <param name="_normal">0.0f ~ 1.0f</param>
	/// <returns>�ŏ��l����ő�l����`��Ԃ���_value�̒l</returns>
	static inline float Lerp(const float _a, const float _b, const float _value)
	{
		float rate{ std::fmaxf(0.0f, std::fminf(_value, 1.0f)) };

		return _a + (_b - _a) * rate;
	}

	/*static inline float LerpForward(const float _a, const float _b, const float _value)
	{
		float rate{ std::fmaxf(0.0f, std::fminf(_value, 1.0f)) };

		return _b + (_a - _b) * rate;
	}*/

	/// <summary>
	/// �ŏ��l�ƍő�l�͈̔͂Ő��K������
	/// </summary>
	/// <param name="_min">�ŏ��l</param>
	/// <param name="_max">�ő�l</param>
	/// <param name="_current">���ݒl</param>
	/// <returns>���K�����ꂽ�l</returns>
	static inline float Normalize(const float _min, const float _max, const float _current)
	{
		massert(_max - _min > FLT_EPSILON
			&& "�ŏ��l�A�ő�l�𓯂��l�ɂł��܂���B�[�����Z���������܂��B @Mathf::Normalize");

		return (_current - _min) / (_max - _min);
	}

	/// <summary>
	/// �����̒��ōŏ��l�����߂�
	/// </summary>
	/// <param name="_a">�la</param>
	/// <param name="_b">�lb</param>
	/// <returns>�����̒��ł̍ŏ��l / 0</returns>
	static float PlusMin(float _a, float _b)
	{
		if (_a < 0 && _b < 0)
		{
			return 0;
		}
		if (_a < 0)
		{
			return _b;
		}
		if (_a < _b)
		{
			return _a;
		}
		else
		{
			return _b;
		}
	}

	/// <summary>
	/// �ˌ��\��
	/// </summary>
	/// <param name="_shotPosition">�����W</param>
	/// <param name="_targetPosition">�^�[�Q�b�g�̍��W</param>
	/// <param name="_targetMove">�^�[�Q�b�g�̈ړ�(/s)</param>
	/// <param name="_bulletSpeed">�e�̑��x</param>
	/// <returns>�����ē�������W</returns>
	static Vector3 TargetingPosition(Vector3 _shotPosition, Vector3 _targetPosition, Vector3 _targetMove, float _bulletSpeed)
	{
		const float BULLET_SPEED{ _bulletSpeed * Time::DeltaTimeF() };
		
		const Vector3 MOVE{ _targetMove * Time::DeltaTimeF() };

		Vector3 toTargetDiff{ _targetPosition - _shotPosition };

		const float A
		{
			(MOVE.x * MOVE.x) + (MOVE.y * MOVE.y) + (MOVE.z * MOVE.z)
			- (BULLET_SPEED * BULLET_SPEED)
		};
		const float B
		{
			2.0f *
			((toTargetDiff.x * MOVE.x) + (toTargetDiff.y * MOVE.y) + (toTargetDiff.z * MOVE.z))
		};
		const float C
		{
			toTargetDiff.x * toTargetDiff.x + toTargetDiff.y * toTargetDiff.y + toTargetDiff.z * toTargetDiff.z
		};

		// 0���Z�h�~
		if (std::fabsf(A) <= FLT_EPSILON)
		{
			if (std::fabsf(B) <= FLT_EPSILON)
			{
				return _targetPosition;
			}
			else
			{
				return _targetPosition + MOVE * (-C / B);
			}
		}

		// ������b��
		float sec{};
		// ��
		float s1{}, s2{};

		// 2���������̉��̌��� ���ʎ�D
		const float D{ B * B - 4.0f * A * C };
		if (D > FLT_EPSILON)
		{
			const float E = std::sqrtf(D);
			s1 = (-B - E) / (2.0f * A);
			s2 = (-B + E) / (2.0f * A);
			sec = PlusMin(s1, s2);
		}
		else  // ������ == ������Ȃ�
		{
			sec = 0;
		}

		return _targetPosition + MOVE * (sec);
	}
}
