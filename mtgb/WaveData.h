#pragma once
#include <xaudio2.h>

namespace mtgb
{
	/// <summary>
	/// Wave�t�@�C���̃f�[�^�i�[�\����
	/// </summary>
	struct WaveData
	{
		WaveData();
		~WaveData();

		WAVEFORMATEX waveFormat;  // �t�H�[�}�b�g���
		byte* pBuffer;            // �f�[�^�o�b�t�@�|�C���^�z��
		size_t bufferSize;        // �f�[�^�o�b�t�@�T�C�Y
	};
}
