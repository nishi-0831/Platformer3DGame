#include "TcpListener.h"
#include "TcpClient.h"

mtnet::TcpListener::TcpListener(const IPEndPoint& _local) :
	Socket{ _local }
{
	if (TryStartup() == false)
	{
		throw "TcpListener�̋N���Ɏ��s";
	}
	if (TryBind() == false)
	{
		throw "TcpListener��IP�o�C���h�Ɏ��s";
	}
}

mtnet::TcpListener::~TcpListener()
{
	Close(true);
}

bool mtnet::TcpListener::TryStartListen()
{
	return Socket::TryStartListen();
}

mtnet::TcpClient* mtnet::TcpListener::AcceptTcpClient()
{
	return static_cast<mtnet::TcpClient*>(Accept());
}
