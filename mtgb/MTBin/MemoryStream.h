#pragma once
#include "Common.h"

namespace mtbin
{
	/// <summary>
	/// <para>Byte�z��̃X�g���[��</para>
	/// </summary>
	class MemoryStream
	{
	public:
		using SeekPoint = size_t;

		enum struct SeekDir : SeekPoint
		{
			Head,  // ��ԓ�
		};

	public:
		MemoryStream(mtbin::Byte* _pBuffer, const size_t& _bufferSize);
		virtual ~MemoryStream();

		/// <summary>
		/// �������X�g���[���ɔC�ӂ̌^����������
		/// </summary>
		/// <typeparam name="T">�C�ӂ̌^</typeparam>
		/// <param name="_value">�������ރI�u�W�F�N�g</param>
		template<typename T>
		void Write(T _value);

		/// <summary>
		/// �������X�g���[���ɔC�ӂ̌^�̔z�����������
		/// </summary>
		/// <param name="_pArray">�z��̃|�C���^</param>
		/// <param name="_arrayLength">�v�f��</param>
		template<typename T>
		void Write(T* _pArray, const int& _arrayLength);

		/// <summary>
		/// �������X�g���[������C�ӂ̌^�T�C�Y���ǂݎ��
		/// </summary>
		/// <typeparam name="T">�C�ӂ̌^</typeparam>
		/// <returns>�ǂݎ�����I�u�W�F�N�g</returns>
		template<typename T>
		T Read();

		/// <summary>
		/// �������X�g���[������C�ӂ̌^�̔z���ǂݎ��
		/// </summary>
		/// <param name="_pWriteBuffer">�ǂݎ���ď������ޔz��̃|�C���^</param>
		/// <param name="_writeBufferLength">�ǂݎ���ď������ޔz��̗v�f��</param>
		/// <param name="_arrayLength">�v�f��</param>
		template<typename T>
		void Read(T* _pWriteBuffer, const int& _writeBufferLength, const int& _arrayLength);

		/// <summary>
		/// �ǂݏ�������ꏊ���ړ�����
		/// </summary>
		/// <param name="_point">�ړ�����ꏊ(��)</param>
		void Seek(SeekPoint _point);
		/// <summary>
		/// �ǂݏ�������ꏊ���ړ�����
		/// </summary>
		/// <param name="_dir">��ΓI�ȏꏊ�w��</param>
		void Seek(SeekDir _dir);

		/// <summary>
		/// ���݂̓ǂݏ����ʒu���擾
		/// </summary>
		/// <returns></returns>
		inline size_t Current() const { return currentIndex; }

		/// <summary>
		/// �S�f�[�^�T�C�Y��Ԃ�
		/// </summary>
		inline size_t GetLength() const { return BUFFER_SIZE; }

		/// <summary>
		/// �o�b�t�@�|�C���^���擾����
		/// </summary>
		/// <returns></returns>
		inline mtbin::Byte* GetBuffer() const { return pBuffer_; }

	private:
		const size_t BUFFER_SIZE;  // �o�b�t�@�z��̃T�C�Y
		mtbin::Byte* pBuffer_;  // �ǂݏ�������o�b�t�@�z��
		size_t currentIndex;  // ���݂̃C���f�b�N�X
	};

	template<typename T>
	inline void MemoryStream::Write(T _value)
	{
		assert((currentIndex + sizeof(T)) < BUFFER_SIZE  // ��������ł��I�[�o�[�������Ȃ�
			&& "buffer over run @mtbin::MemoryStream::Write");

		void* dist = &(pBuffer_[currentIndex]);
		void* src = reinterpret_cast<void*>(&_value);
		size_t size = sizeof(T);
		// ��������
		::memcpy(dist, src, size);
		currentIndex += sizeof(T);  // �T�C�Y���i�߂�
	}

	template<typename T>
	inline void MemoryStream::Write(T* _pArray, const int& _arrayLength)
	{
		size_t size{ sizeof(T) * _arrayLength };  // �������ރT�C�Y
		assert((currentIndex + size) <= BUFFER_SIZE  // ��������ł��I�[�o�[�������Ȃ�
			&& "buffer over run @mtbin::MemoryStream::Write");

		// ��������
		::memcpy(
			&(pBuffer_[currentIndex]),
			reinterpret_cast<mtbin::Byte*>(_pArray),
			size);
		currentIndex += size;  // �T�C�Y���i�߂�
	}

	template<typename T>
	inline T MemoryStream::Read()
	{
		assert((currentIndex + sizeof(T)) <= BUFFER_SIZE  // �ǂݍ���ł��A���_�[�������Ȃ�
			&& "buffer under run @mtbin::MemoryStream::Read");

		T pickBuffer{};  // ���o���p�o�b�t�@�p��
		::memcpy(&pickBuffer, &(pBuffer_[currentIndex]), sizeof(T));  // ���o��
		currentIndex += sizeof(T);  // �T�C�Y���i�߂�

		return pickBuffer;
	}

	template<typename T>
	inline void MemoryStream::Read(T* _pWriteBuffer, const int& _writeBufferLength, const int& _arrayLength)
	{
		assert(_pWriteBuffer != nullptr  // �������ݐ��nullptr�ł͂Ȃ�
			&& "write buffer is nullptr @mtbin::MemoryStream::Read");

		assert(_writeBufferLength >= _arrayLength  // �������ݐ�ɏ\���ȗe�ʂ���
			&& "write buffer under run @mtbin::MemoryStream::Read");

		size_t size{ sizeof(T) * _arrayLength };  // �ǂݍ��ރT�C�Y
		assert((currentIndex + size) <= BUFFER_SIZE  // �ǂݍ���ł��A���_�[�������Ȃ�
			&& "buffer under run @mtbin::MemoryStream::Read");

		::memcpy(_pWriteBuffer, &(pBuffer_[currentIndex]), size);  // ���o��
		currentIndex += size;  // �T�C�Y���i�߂�
	}
}
