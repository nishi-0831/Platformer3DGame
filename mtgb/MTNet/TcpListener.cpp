#include "TcpListener.h"
#include "TcpClient.h"

mtnet::TcpListener::TcpListener(const IPEndPoint& _local)
	: Socket{_local}
{
	if (TryStartup() == false)
	{
		throw "TcpListenerの起動に失敗";
	}
	if (TryBind() == false)
	{
		throw "TcpListenerのIPバインドに失敗";
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
