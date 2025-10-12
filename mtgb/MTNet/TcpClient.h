#pragma once
#include "Socket.h"

namespace mtnet
{
	/// <summary>
	/// TCP�ʐM�����邽�߂̃N���C�A���g
	/// </summary>
	class TcpClient : public Socket
	{
	public:
		/// <summary>
		/// Tcp�N���C�A���g���쐬����
		/// </summary>
		/// <param name="_local">�ڑ����̃G���h�|�C���g</param>
		TcpClient(const IPEndPoint& _local);
		~TcpClient();

		/// <summary>
		/// �ڑ�����
		/// </summary>
		/// <param name="_remote">�ڑ���̃G���h�|�C���g</param>
		void Connect(const IPEndPoint& _remote);

		ReceivedLength Receive(Byte* _buffer, const int& _bufferLength);
		ReceivedLength Send(Byte* _buffer, const int& _bufferLength);
	};
}
