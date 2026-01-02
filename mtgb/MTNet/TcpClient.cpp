#include "TcpClient.h"

namespace mtnet
{
	mtnet::TcpClient::TcpClient(const IPEndPoint& _local)
		: Socket{_local}
	{
		if (TryStartup() == false)
		{
			throw "TcpClientの作成に失敗";
		}
	}

	mtnet::TcpClient::~TcpClient()
	{
		Close(true);
	}

	void mtnet::TcpClient::Connect(const IPEndPoint& _remote)
	{
		if (TryConnect(_remote) == false)
		{
			throw "接続に失敗";
		}
	}

	ReceivedLength mtnet::TcpClient::Receive(Byte* _buffer, const int& _bufferLength)
	{
		return Socket::Receive(_buffer, _bufferLength);
	}

	ReceivedLength mtnet::TcpClient::Send(Byte* _buffer, const int& _bufferLength)
	{
		if (TrySend(_buffer, _bufferLength) == true)
		{
			return _bufferLength;
		}
		else
		{
			return -1;
		}
	}
} // namespace mtnet
