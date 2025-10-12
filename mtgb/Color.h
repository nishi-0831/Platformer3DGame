#pragma once
#include <cstdint>
#include "Vector3.h"
#include "Vector4.h"

namespace mtgb
{
	/// <summary>
	/// �F��0~255�͈̔͂ŕ\������\����
	/// </summary>
	struct Color
	{
		/// <summary>
		/// ��\�I�ȐF�R�[�h
		/// </summary>
		enum : uint32_t
		{
			RED   = 0xff0000,   // �ԐF
			GREEN = 0x00ff00,   // �ΐF
			BLUE  = 0x0000ff,   // �F
			WHITE  = 0xffffff,  // ���F
			BLACK  = 0x000000,  // ���F
		};

		/// <summary>
		/// �F�̊e�����ɃA�N�Z�X�p
		/// </summary>
		enum struct Component : int32_t
		{
			Blue,   // �F����
			Green,  // �ΐF����
			Red,    // �ԐF����
			Alpha,  // ���ߓx����
			Max,    // �F�����̎�ސ�
		};

		/// <summary>
		/// �F�̊e��������F�����
		/// </summary>
		/// <param name="_r">�Ԑ���(0~255)</param>
		/// <param name="_g">�ΐ���(0~255)</param>
		/// <param name="_b">����(0~255)</param>
		/// <param name="_alpha">�����x����(0~255)</param>
		Color(
			const uint8_t _r,
			const uint8_t _g,
			const uint8_t _b,
			const uint8_t _alpha = UINT8_MAX);

		/// <summary>
		/// �J���[�R�[�h����F�����
		/// </summary>
		/// <param name="_code">16�i���J���[�R�[�h</param>
		Color(uint32_t _code);

		/// <summary>
		/// �J���[�R�[�h��unsigned int �ɃL���X�g
		/// </summary>
		inline operator uint32_t() const noexcept { return code; }

		/// <summary>
		/// �����x�����̐��K�������l���擾
		/// </summary>
		/// <returns>�����x�����̐��K�����ꂽ�l</returns>
		inline float GetAlphaNorm() const
		{
			return static_cast<float>(component[static_cast<int32_t>(Component::Alpha)]) / UINT8_MAX;
		}
		/// <summary>
		/// �e�F�����𐳋K������3�����x�N�g�����擾
		/// </summary>
		/// <returns>RGB�𐳋K������3�����x�N�g��</returns>
		inline Vector3 ToVector3Norm() const
		{
			return
			{
				static_cast<float>(component[static_cast<int32_t>(Component::Red)]) / UINT8_MAX,
				static_cast<float>(component[static_cast<int32_t>(Component::Green)]) / UINT8_MAX,
				static_cast<float>(component[static_cast<int32_t>(Component::Blue)]) / UINT8_MAX,
			};
		}
		/// <summary>
		/// �e�F����+�����x�����𐳋K������4�����x�N�g�����擾
		/// </summary>
		/// <returns>RGBA�𐳋K������3�����x�N�g��</returns>
		inline Vector4 ToVector4Norm() const
		{
			return
			{
				static_cast<float>(component[static_cast<int32_t>(Component::Red)]) / UINT8_MAX,
				static_cast<float>(component[static_cast<int32_t>(Component::Green)]) / UINT8_MAX,
				static_cast<float>(component[static_cast<int32_t>(Component::Blue)]) / UINT8_MAX,
				static_cast<float>(component[static_cast<int32_t>(Component::Alpha)]) / UINT8_MAX,
			};
		}

		union
		{
			uint32_t code;  // 16�i���J���[�R�[�h�p
			uint8_t component[sizeof(uint32_t) / sizeof(uint8_t)];  // 1�o�C�g���̃J���[�v�f�p
		};
	};
}
