#pragma once
#include <string>
#include <sstream>
#include <cassert>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "MTBin/Common.h"

#pragma comment(lib, "ws2_32.lib")

namespace mtnet
{
	// IP�A�h���X
	using IPAddress = std::string;
	// �|�[�g�ԍ�
	using PortNumber = unsigned short;
	// ���ʃR�[�h
	using ResultCode = int;
	// �󂯎�����o�C�g��
	using ReceivedLength = int;
}
