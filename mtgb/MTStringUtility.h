#pragma once
#include <string>
#include <string_view>
#include "MTAssert.h"

namespace mtgb
{
	/// <summary>
	/// UTF8 ������(�o�C�g���ł͂Ȃ�)���擾����
	/// </summary>
	/// <param name="_string">utf8�̕�����</param>
	/// <returns>������</returns>
	static inline size_t GetSizeUTF8Characters(const std::u8string& _string)
	{
		enum BYTE_SIZE : int
		{
			BYTE_SIZE_IDK,
			BYTE_SIZE1,
			BYTE_SIZE2,
			BYTE_SIZE3,
			BYTE_SIZE4,
		};

		static auto isMatchByteSize
		{
			[](const uint8_t _byte, const uint8_t _idByte, const uint8_t _byteMask) -> bool
			{
				return (_byte & _byteMask) == _idByte;
			}
		};
		
		// REF: https://ja.wikipedia.org/wiki/UTF-8
		size_t charasCount{};
		for (int byteIndex = 0; byteIndex < _string.size();)
		{
			BYTE_SIZE charaByteSize{};
			if (isMatchByteSize(_string[byteIndex], 0b0000'0000, 0b1000'0000))
			{
				charaByteSize = BYTE_SIZE1;
			}
			else if (isMatchByteSize(_string[byteIndex], 0b1100'0000, 0b1110'0000))
			{
				charaByteSize = BYTE_SIZE2;
			}
			else if (isMatchByteSize(_string[byteIndex], 0b1110'0000, 0b1111'0000))
			{
				charaByteSize = BYTE_SIZE3;
			}
			else if (isMatchByteSize(_string[byteIndex], 0b1111'0000, 0b1111'1000))
			{
				charaByteSize = BYTE_SIZE4;
			}

			massert(charaByteSize != BYTE_SIZE_IDK && "�o�C�g�J�E���g���s��v @SubStrBegin");

			byteIndex += charaByteSize;
			charasCount++;
		}

		return charasCount;
	}

	/// <summary>
	/// utf8�������؂���
	/// </summary>
	/// <param name="_string">utf8������</param>
	/// <param name="_count">������(�o�C�g���ł͂Ȃ�)</param>
	/// <returns>�؂���ꂽutf8������</returns>
	static inline std::u8string SubStrBegin(const std::u8string& _string, const int _count)
	{
		enum BYTE_SIZE : int
		{
			BYTE_SIZE_IDK,
			BYTE_SIZE1,
			BYTE_SIZE2,
			BYTE_SIZE3,
			BYTE_SIZE4,
		};

		int charaCount{};  // utf8�̕������J�E���g
		int byteCount{};  // 1�������\������o�C�g���J�E���g
		int byteIndex{};

		static auto isMatchByteSize
		{
			[](const uint8_t _byte, const uint8_t _idByte, const uint8_t _byteMask) -> bool
			{
				return (_byte & _byteMask) == _idByte;
			}
		};

		// REF: https://ja.wikipedia.org/wiki/UTF-8
		for (byteIndex = 0; charaCount < _count && byteIndex < _string.size();)
		{
			BYTE_SIZE charaByteSize{};
			if (isMatchByteSize(_string[byteIndex], 0b0000'0000, 0b1000'0000))
			{
				charaByteSize = BYTE_SIZE1;
			}
			else if (isMatchByteSize(_string[byteIndex], 0b1100'0000, 0b1110'0000))
			{
				charaByteSize = BYTE_SIZE2;
			}
			else if (isMatchByteSize(_string[byteIndex], 0b1110'0000, 0b1111'0000))
			{
				charaByteSize = BYTE_SIZE3;
			}
			else if (isMatchByteSize(_string[byteIndex], 0b1111'0000, 0b1111'1000))
			{
				charaByteSize = BYTE_SIZE4;
			}

			massert(charaByteSize != BYTE_SIZE_IDK && "�o�C�g�J�E���g���s��v @SubStrBegin");

			byteIndex += charaByteSize;
			charaCount++;
		}

		return _string.substr(0, byteIndex);
	}

	/// <summary>
	/// ���C�h������ɕϊ�����
	/// </summary>
	/// <param name="_cstr"></param>
	/// <returns></returns>
	/*static inline std::wstring ToWString(char* _cstr)
	{
		std::string str{ _cstr };
		return ToWString(str);
	}*/

	/// <summary>
	/// ���C�h������ɕϊ�����
	/// </summary>
	/// <param name="_string"></param>
	/// <returns></returns>
	static inline std::wstring ToWString(const std::string& _string)
	{
		return { _string.begin(), _string.end() };
	}

	/// <summary>
	/// ���C�h������ɕϊ�����
	/// </summary>
	/// <param name="_stringView"></param>
	/// <returns></returns>
	static inline std::wstring ToWString(const std::string_view& _stringView)
	{
		return { _stringView.begin(), _stringView.end() };
	}

	/// <summary>
	/// �}���`�o�C�g�����񂩂烏�C�h������֕ϊ�
	/// </summary>
	/// <param name="_string">�ϊ����镶����</param>
	/// <returns></returns>
	std::wstring MultiToWide(const std::string& _string);
	/// <summary>
	/// �}���`�o�C�g�����񂩂烏�C�h������֕ϊ�
	/// </summary>
	/// <param name="_string">������</param>
	/// <returns></returns>
	std::wstring MultiToWide(const char* _string);

	std::string WideToMulti(const std::wstring& _wstring);
	std::string WideToMulti(const wchar_t* _wstring);

	/// <summary>
	/// �}���`�o�C�g�����񂩂烏�C�h������֕ϊ�
	/// </summary>
	/// <param name="_string">�ϊ����镶����</param>
	/// <returns></returns>
	std::string MultiToUTF8(const std::string & _string);
	/// <summary>
	/// �}���`�o�C�g�����񂩂烏�C�h������֕ϊ�
	/// </summary>
	/// <param name="_string">�ϊ����镶����</param>
	/// <returns></returns>
	std::string MultiToUTF8(const char* _string);

	std::wstring UTF8ToWide(const std::string& _string);
	std::wstring UTF8ToWide(const std::u8string& _string);
	std::wstring UTF8ToWide(const char* _string);

	std::string WideToUTF8(const std::wstring& _wstring);
	std::string WideToUTF8(const wchar_t* _wstring);

	std::string UTF8ToMulti(const std::string & _string);
	std::string UTF8ToMulti(const char * _string);
}
