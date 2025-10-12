#pragma once
#include "Common.h"
#include "EndPoint.h"

namespace mtnet
{
	using mtbin::Byte;

	class Socket
	{
	public:
		Socket(const IPEndPoint& _local);
		~Socket();

		/// <summary>
		/// �\�P�b�g�����
		/// </summary>
		/// <param name="force">�����I�ɕ��� true / false</param>
		void Close(const bool& force = false);

	protected:
		/// <summary>
		/// �����ɋN������
		/// </summary>
		/// <returns>�N���ł��� true / false</returns>
		bool TryStartup();

		/// <summary>
		/// �����Ƀo�C���h����
		/// </summary>
		/// <returns>�������� true / false</returns>
		bool TryBind();

		/// <summary>
		/// ������TCP�󂯓�����J�n����
		/// </summary>
		/// <returns>�������� true / false</returns>
		bool TryStartListen();

		/// <summary>
		/// �����[�g�\�P�b�g���󂯓����
		/// </summary>
		/// <returns>�󂯓��ꂽ�\�P�b�g</returns>
		Socket* Accept();

		/// <summary>
		/// �����ɐڑ�����
		/// </summary>
		/// <param name="_remote">�O���̃G���h�|�C���g</param>
		/// <returns>�ڑ��ł��� true / false</returns>
		bool TryConnect(const IPEndPoint& _remote);

		/// <summary>
		/// �󂯎��
		/// </summary>
		/// <param name="buffer">��M�o�b�t�@</param>
		/// <param name="bufferLength">�o�b�t�@�̒���</param>
		/// <returns>���ۂɎg�����o�b�t�@�̒���</returns>
		mtnet::ReceivedLength Receive(Byte* buffer, const int& bufferLength);

		/// <summary>
		/// �����ɑ��M����
		/// </summary>
		/// <param name="buffer">���M�o�b�t�@</param>
		/// <param name="bufferLength">�o�b�t�@�̒���</param>
		/// <returns>���M�ł��� true / false</returns>
		bool TrySend(Byte* buffer, const int& bufferLength);

		/// <summary>
		/// ���݂̌��ʃR�[�h���擾����
		/// </summary>
		/// <returns>���ʃR�[�h</returns>
		inline ResultCode GetResultCode() const { return result_; }

	public:
		const IPEndPoint LOCAL_IP_END_POINT;

	protected:
		ResultCode result_;

	private:
		WSADATA wsaData_;
		SOCKADDR_IN addr_;
		SOCKET connectingSocket_;
	};
}
