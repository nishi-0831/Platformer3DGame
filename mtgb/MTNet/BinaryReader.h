#pragma once
#include "../MTBin/MemoryStream.h"

namespace mtnet
{
	using mtbin::MemoryStream;

	/// <summary>
	/// MemoryStream�̓ǂݎ���p��
	/// </summary>
	class BinaryReader : public MemoryStream
	{
	public:
		BinaryReader(
			mtbin::Byte* _pReadBuffer,
			const size_t& _bufferSize);
		~BinaryReader();

		using MemoryStream::Read;

	private:  // �ǂݎ��ȊO�̓v���C�x�[�g�ɂ���
		using MemoryStream::Write;
		using MemoryStream::Seek;
	};
}
