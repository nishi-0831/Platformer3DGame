#pragma once
#include <string_view>

namespace mtgb
{
	namespace FilePath
	{
		/// <summary>
		/// �t�@�C�����g���q���擾����
		/// </summary>
		/// <param name="_fileName">�t�@�C����</param>
		/// <returns>�g���q�̕�����</returns>
		inline std::string_view GetExtension(const std::string_view& _fileName)
		{
			size_t extPos{ _fileName.find_last_of('.') };
			return _fileName.substr(extPos, _fileName.size() - extPos);
		}
	}
}
