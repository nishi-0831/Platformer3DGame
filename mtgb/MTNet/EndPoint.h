#pragma once
#include "Common.h"


//typedef struct SOCKADDR_IN* PSOCKADDR_IN;
// �G���h�|�C���g: �N���C�A���g�ƃT�[�r�X�Ђ�����߂�

namespace mtnet
{
	/// <summary>
	/// �G���h�|�C���g�̏��
	/// </summary>
	class IPEndPoint
	{
	public:
		/// <summary>
		/// �G���h�|�C���g�̏��
		/// </summary>
		/// <param name="_ipAddress">IP�A�h���X</param>
		/// <param name="_portNumber">�|�[�g�ԍ�</param>
		IPEndPoint(const IPAddress& _ipAddress, PortNumber _portNumber);
		~IPEndPoint();

		mtnet::IPAddress GetIPAddress();

		inline void SetIPAddress(const IPAddress& _ipAddress) { ipAddress_ = _ipAddress; }
		inline void SetPortNumber(PortNumber _portNumber) { portNumber_ = _portNumber; }
		inline PortNumber GetPortNumber() const { return portNumber_; }
		bool TryGenerateSockAddrIn(SOCKADDR_IN* _out) const;
		bool TryGetOctets(
			byte* _octet1,
			byte* _octet2,
			byte* _octet3,
			byte* _octet4) const;

	private:
		PortNumber portNumber_;
		IPAddress ipAddress_;
		ADDRINFO addressInfo_;
	};
}
