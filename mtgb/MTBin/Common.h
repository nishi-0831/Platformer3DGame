#pragma once
#include <cstdint>
#include <cassert>

namespace mtbin
{
	/// <summary>
	/// mtbin�̃o�C�g�^
	/// </summary>
	using Byte = unsigned char;

	namespace Utility
	{
		/// <summary>
		/// �`�����NId���r����
		/// </summary>
		/// <typeparam name="SIZE">Id�̃o�C�g��</typeparam>
		/// <param name="_header">�`�����NId�i�[�z��</param>
		/// <param name="_str">��rId������</param>
		/// <returns>���v true / false</returns>
		template<size_t SIZE>
		inline bool CompareId(const Byte _header[SIZE], const char* _str)
		{
			for (size_t i = 0; i < SIZE; i++)
			{
				if (_header[i] != _str[i])
				{
					return false;
				}
			}
			return true;
		}
	}
}
