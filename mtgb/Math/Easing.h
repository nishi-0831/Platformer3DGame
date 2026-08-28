#pragma once
#include <cmath>

namespace mtgb
{
	/// <summary>
	/// <para>イージング関数</para>
	/// <para>https://easings.net/ja</para>
	/// </summary>
	struct Ease
	{
		static inline float OutExpo(float _x)
		{
			// https://easings.net/ja#easeOutExpo
			return _x == 1.0f ? 1.0f : 1.0f - std::powf(2.0f, -10.0f * _x);
		}

		static inline float OutQuart(float _x)
		{
			// https://easings.net/ja#easeOutQuart
			return 1.0f - std::powf(1.0f - _x, 4.0f);
		}

		static inline float OutBounce(float _x)
		{
			// https://easings.net/ja#easeOutBounce
			static const float n1 = 7.5625f;
			static const float d1 = 2.75f;

			if (_x < 1.0f / d1)
			{
				return n1 * _x * _x;
			}
			else if (_x < 2.0f / d1)
			{
				return n1 * (_x -= 1.5f / d1) * _x + 0.75f;
			}
			else if (_x < 2.5f / d1)
			{
				return n1 * (_x -= 2.25f / d1) * _x + 0.9375f;
			}
			else
			{
				return n1 * (_x -= 2.625f / d1) * _x + 0.984375f;
			}
		}
	};
} // namespace mtgb
