#pragma once
#include <string>
#include <winsock2.h>
#include <functional>
#include <vector>
#include "HttpHeaderBuilder.h"

#pragma comment(lib,"ws2_32.lib")

namespace mtnet
{
	class HttpClient
	{
	public:
		using ResponceCallback = std::function<void(const std::string&)>;
		using BinaryResponceCallback = std::function<void(std::vector<uint8_t>&)>;

	public:
		HttpClient();
		~HttpClient();

		/// <summary>
		/// <para>�ڑ����URL���w�肵�ď�����</para>
		/// <para>�ڑ����ύX���邽�тɌĂяo��</para>
		/// </summary>
		/// <param name="_remoteUrl">�ڑ����URL (X.X.X.X:Port)</param>
		void Initialize(const std::string& _remoteUrl);

		/// <summary>
		/// �|�X�g���N�G�X�g�𑗐M����
		/// </summary>
		/// <param name="_header">�w�b�_</param>
		/// <param name="_pathAndQuery">http://*.*/{���̕���}</param>
		/// <param name="_body">�{��(�C��)</param>
		void Post(
			HttpHeaderBuilder& _header,
			const std::string& _pathAndQuery,
			const ResponceCallback& _onResponce,
			const std::string& _body = "");

		std::string Post(
			HttpHeaderBuilder& _header,
			const std::string& _pathAndQuery,
			const std::string& _body = "");

		void PostAndBinaryResponce(
			HttpHeaderBuilder& _header,
			const std::string& _pathAndQuery,
			const BinaryResponceCallback& _onRecponce,
			const std::string& _body = "");

		/// <summary>
		/// WSA�̃N���[���A�b�v (�A�v���P�[�V���������Ƃ��ɌĂ�)
		/// </summary>
		void Cleanup();

	public:
		/// <summary>
		/// <para>��������p�[�Z���gURI�ɕϊ�����</para>
		/// <para>URI���œ��{��Ȃǂ� %�����R�[�h �ɂȂ�</para>
		/// </summary>
		/// <param name="str"></param>
		/// <returns></returns>
		static std::string ToPercentURI(const std::string& str);

	private:
		/// <summary>
		/// ������URL��IP�A�h���X�ƃ|�[�g�ԍ��ɕ�������
		/// </summary>
		/// <param name="_srcUrl">����URL</param>
		/// <param name="_pOctet1">�|�C���^�n��:IP�A�h���X��1�I�N�e�b�g��</param>
		/// <param name="_pOctet2">�|�C���^�n��:IP�A�h���X��2�I�N�e�b�g��</param>
		/// <param name="_pOctet3">�|�C���^�n��:IP�A�h���X��3�I�N�e�b�g��</param>
		/// <param name="_pOctet4">�|�C���^�n��:IP�A�h���X��4�I�N�e�b�g��</param>
		/// <param name="_pPortNumber">�|�C���^�n��:�|�[�g�ԍ�</param>
		/// <returns>�������� true / false</returns>
		static bool TryToIPAddresAndPort(
			const std::string& _srcUrl,
			BYTE* _pOctet1,
			BYTE* _pOctet2,
			BYTE* _pOctet3,
			BYTE* _pOctet4,
			USHORT* _pPortNumber);

	private:
		WSADATA wsaData_;  // WinSock2�Ńl�b�g���[�N���g�����߂�
		SOCKET socket_;  // �ʐM����\�P�b�g
		SOCKADDR_IN sockAddr_;  // �ʐM��̃G���h�|�C���g���
		std::string remoteUri_;  // �ڑ����URI
	};
}
