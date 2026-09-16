#include "MemoryStream.h"
#include <utility>
#include "Utility/ReleaseUtility.h"
mtbin::MemoryStream::MemoryStream(mtbin::Byte* _pBuffer, size_t _bufferSize)
	: BUFFER_SIZE { _bufferSize }
	, pBuffer_ { _pBuffer }
	, currentIndex { 0 }
{
}

mtbin::MemoryStream::MemoryStream(MemoryStream&& _other) noexcept
	: BUFFER_SIZE { _other.BUFFER_SIZE }
	, pBuffer_ { _other.pBuffer_ }
	, currentIndex { _other.currentIndex }
{
	_other.pBuffer_ = nullptr;
}

mtbin::MemoryStream::~MemoryStream()
{
	SAFE_DELETE(pBuffer_);
}

void mtbin::MemoryStream::Seek(SeekPoint _point)
{
	// 範囲制御
	if (_point < 0)
	{
		_point = 0;
	}
	else if (_point > BUFFER_SIZE)
	{
		_point = BUFFER_SIZE;
	}

	currentIndex = _point;
}

void mtbin::MemoryStream::Seek(SeekDir _dir)
{
	switch (_dir)
	{
		case SeekDir::HEAD :
			currentIndex = 0;
			break;
		default :
			break;
	}
}
