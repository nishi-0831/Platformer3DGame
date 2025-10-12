#pragma once
#include <xaudio2.h>
#include <string>
#include "MTBin/MemoryStream.h"

typedef struct tWAVEFORMATEX WAVEFORMATEX;

namespace mtgb
{
	struct WaveData;
	class AudioPlayer;
	class Audio;

	/// <summary>
	/// <para>�����N���b�v (=�����f�[�^)</para>
	/// <para>TODO: Wave�ȊO���ǂݍ��ޏꍇ�A������g��</para>
	/// </summary>
	class AudioClip
	{
		friend AudioPlayer;
		friend Audio;

	public:
		AudioClip();
		~AudioClip();

		/// <summary>
		/// �����f�[�^���N���b�v�Ƀ��[�h����
		/// </summary>
		/// <param name="_ms">�����f�[�^��ms</param>
		void Load(mtbin::MemoryStream& _ms);

		/// <summary>
		/// WAV�ǂݍ���
		/// </summary>
		/// <param name="_ms">WAV�̃������X�g���[��</param>
		/// <param name="_first4"> �ŏ���4�o�C�g�̎��ʎq</param>
		void LoadWave(mtbin::MemoryStream& _ms, const byte* _first4);

		/// <summary>
		/// mp3�ǂݍ���
		/// </summary>
		/// <param name="_ms">mp3�̃������X�g���[��</param>
		/// <param name="_first4">�ŏ���4�o�C�g�̎��ʎq</param>
		void LoadMp3(mtbin::MemoryStream& _ms, const byte* _first4);

		/// <summary>
		/// �����f�[�^�̑��Đ�����(�b)���擾
		/// </summary>
		/// <returns></returns>
		float GetTotalTimeSec() const;
	
	private:
		WaveData* pWaveData_;  // Wave�t�@�C���̃f�[�^
	};
}
