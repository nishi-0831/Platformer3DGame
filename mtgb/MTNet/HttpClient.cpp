#include "HttpClient.h"
#include <cassert>
#include <iomanip>

namespace
{
	static const size_t BUFFER_SIZE{ 1024 };
}

mtnet::HttpClient::HttpClient() :
	wsaData_{},
	socket_{ INVALID_SOCKET },  // �����\�P�b�g�����Ă���
	sockAddr_{}
{
}

mtnet::HttpClient::~HttpClient()
{
}

void mtnet::HttpClient::Initialize(const std::string& _remoteUrl)
{
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData_);
	assert(result == 0 &&  // WSA�̋N���͐������Ă���
		"WSAStartup failed.");

	if (socket_ != INVALID_SOCKET)
	{
		closesocket(socket_);  // ���������\�P�b�g�ł͂Ȃ��Ȃ���Ă�����
		socket_ = INVALID_SOCKET;
	}

	// MEMO: ����DNS��������Ȃ�
	//     : locate host = gethostbyname(_remoteUrl.c_str());
	// NOTE: �������A�ȉ���define���K�v����
	//     : _WINSOCK_DEPRECATED_NO_WARNINGS

	sockAddr_ = {};
	sockAddr_.sin_family = AF_INET;

	USHORT portNumber;
	bool succeed = TryToIPAddresAndPort(
		_remoteUrl,
		&sockAddr_.sin_addr.S_un.S_un_b.s_b1,
		&sockAddr_.sin_addr.S_un.S_un_b.s_b2,
		&sockAddr_.sin_addr.S_un.S_un_b.s_b3,
		&sockAddr_.sin_addr.S_un.S_un_b.s_b4,
		&portNumber);
	
	// NOTE: �|�[�g�ԍ��̓r�b�O�G���f�B�A���ł��邽�߁Ahtons�֐���ʂ��K�v������
	sockAddr_.sin_port = htons(portNumber);  // �|�[�g�ԍ����r�b�O�G���f�B�A���ɕϊ�����

	remoteUri_ = _remoteUrl;
}

void mtnet::HttpClient::Post(
	HttpHeaderBuilder& _header,
	const std::string& _pathAndQuery,
	const ResponceCallback& _onResponce,
	const std::string& _body)
{
	_onResponce(Post(_header, _pathAndQuery, _body));
}

std::string mtnet::HttpClient::Post(HttpHeaderBuilder& _header, const std::string& _pathAndQuery, const std::string& _body)
{
	std::ostringstream getHttp{};

	getHttp << "POST /" << _pathAndQuery << " HTTP/1.1" << std::endl;
	getHttp << "Host: " << remoteUri_ << std::endl;
	getHttp << "Connection: close" << std::endl;
	getHttp << "Content-Length: " << _body.size() << std::endl;
	getHttp << _header.oss_.str() << std::endl;
	getHttp << _body << std::endl;

	std::string buf = getHttp.str();

	int result{};

	socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	assert(socket_ != INVALID_SOCKET  // �\�P�b�g�͐���ɐ������ꂽ
		&& "�����ȃ\�P�b�g���������ꂽ");

	result = connect(socket_, reinterpret_cast<SOCKADDR*>(&sockAddr_), sizeof(SOCKADDR_IN));
	
	if (result != 0)
	{
		int error = WSAGetLastError();
		printf("");
	}
	
	assert(result == 0  // �ڑ��ł��Ă���
		&& "Could not connect");

	result = send(socket_, getHttp.str().c_str(), strlen(getHttp.str().c_str()), NULL);

	assert(result > 0  // ���M�ł��Ă���
		&& "Failed send");

	char buffer[BUFFER_SIZE]{};  // ��M�o�b�t�@

	int receiveLength{};
	std::string receiveString{};

	while (true)
	{
		receiveLength = recv(socket_, buffer, BUFFER_SIZE, NULL);
		if (receiveLength == 0)  // 0 �Ȃ�~�߂�
		{
			break;
		}
		for (int i = 0; i < receiveLength; i++)
		{
			receiveString.push_back(buffer[i]);
		}
	}

	result = closesocket(socket_);
	assert(result == 0  // �����Ă���
		&& "close error");
	socket_ = INVALID_SOCKET;

	return receiveString;
}

void mtnet::HttpClient::PostAndBinaryResponce(
	HttpHeaderBuilder& _header,
	const std::string& _pathAndQuery,
	const BinaryResponceCallback& _onRecponce,
	const std::string& _body)
{
	std::ostringstream getHttp{};

	getHttp << "POST /" << _pathAndQuery << " HTTP/1.1" << std::endl;
	getHttp << "Host: " << remoteUri_ << std::endl;
	getHttp << "Connection: close" << std::endl;
	getHttp << "Content-Length: " << _body.size() << std::endl;
	getHttp << _header.oss_.str() << std::endl;
	getHttp << _body << std::endl;

	std::string buf = getHttp.str();

	int result{};

	socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	assert(socket_ != INVALID_SOCKET  // �\�P�b�g�͐���ɐ������ꂽ
		&& "�����ȃ\�P�b�g���������ꂽ");

	result = connect(socket_, reinterpret_cast<SOCKADDR*>(&sockAddr_), sizeof(SOCKADDR_IN));

	if (result != 0)
	{
		int error = WSAGetLastError();
		printf("");
	}

	assert(result == 0  // �ڑ��ł��Ă���
		&& "Could not connect");

	result = send(socket_, getHttp.str().c_str(), strlen(getHttp.str().c_str()), NULL);

	assert(result > 0  // ���M�ł��Ă���
		&& "Failed send");

	char buffer[BUFFER_SIZE]{};  // ��M�o�b�t�@

	int receiveLength{};
	std::vector<unsigned char> buildBuffer{};

	while (true)
	{
		receiveLength = recv(socket_, buffer, BUFFER_SIZE, NULL);
		if (receiveLength <= 0)  // 0 �ȉ��Ȃ�~�߂�
		{
			break;
		}

		buildBuffer.insert(buildBuffer.end(), buffer, buffer + receiveLength);
	}

	result = closesocket(socket_);
	if (result != 0)  // �����Ă���
	{
		return;
	}
	//assert(result == 0  // �����Ă���
	//	&& "close error");
	socket_ = INVALID_SOCKET;

	_onRecponce(buildBuffer);
}

void mtnet::HttpClient::Cleanup()
{
	if (socket_ != INVALID_SOCKET)
	{
		closesocket(socket_);  // ���������\�P�b�g�ł͂Ȃ��Ȃ���Ă�����
		socket_ = INVALID_SOCKET;
	}
	WSACleanup();  // WSA�̃N���[���A�b�v
}

bool mtnet::HttpClient::TryToIPAddresAndPort(
	const std::string& _srcUrl,
	BYTE* _pOctet1,
	BYTE* _pOctet2,
	BYTE* _pOctet3,
	BYTE* _pOctet4,
	USHORT* _pPortNumber)
{
	std::istringstream iss{ _srcUrl };
	std::string splitString{};

	if (!std::getline(iss, splitString, '.'))
	{
		return false;
	}
	*_pOctet1 = static_cast<UCHAR>(std::stoul(splitString));
	if (!std::getline(iss, splitString, '.'))
	{
		return false;
	}
	*_pOctet2 = static_cast<UCHAR>(std::stoul(splitString));
	if (!std::getline(iss, splitString, '.'))
	{
		return false;
	}
	*_pOctet3 = static_cast<UCHAR>(std::stoul(splitString));
	if (!std::getline(iss, splitString, ':'))
	{
		return false;
	}
	*_pOctet4 = static_cast<UCHAR>(std::stoul(splitString));
	if (!std::getline(iss, splitString, '/'))
	{
		return false;
	}
	*_pPortNumber = static_cast<USHORT>(std::stoul(splitString));

	return true;
}

std::string mtnet::HttpClient::ToPercentURI(const std::string& str)
{
	std::ostringstream encoded;

	for (unsigned char c : str)
	{
		if (('0' <= c && c <= '9') ||
			('A' <= c && c <= 'Z') ||
			('a' <= c && c <= 'z') ||
			c == '-' || c == '_' || c == '.' || c == '~')
		{
			encoded << c;
		}
		else
		{
			encoded << '%' << std::uppercase << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(c);
		}
	}
	return encoded.str();
}
