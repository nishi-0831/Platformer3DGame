#pragma once
#include "Socket.h"

namespace mtnet
{
	class TcpClient;

	/// <summary>
	/// TCP�ʐM�����郊�X�i�[ (TCPClient���m��ʐM������)
	/// </summary>
	class TcpListener : public Socket
	{
	public:
		TcpListener(const IPEndPoint& _local);
		~TcpListener();

		bool TryStartListen();

		TcpClient* AcceptTcpClient();

	private:
	};
}
