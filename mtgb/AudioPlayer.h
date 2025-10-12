#pragma once
#include "IComponent.h"
#include "AudioPlayerCP.h"
#include "Handlers.h"

struct IXAudio2SourceVoice;

namespace mtgb
{
	class AudioClip;
	class AudioPlayerCP;

	/// <summary>
	/// �������Đ�������
	/// </summary>
	class AudioPlayer : public IComponent<AudioPlayerCP, AudioPlayer>
	{
		friend AudioPlayerCP;

	public:
		using IComponent<mtgb::AudioPlayerCP, AudioPlayer>::IComponent;
		//AudioPlayer();
		~AudioPlayer();

		inline AudioPlayer& operator=(const AudioPlayer& _other)
		{
			if (&_other == this)
			{
				return *this;
			}
			this->pSourceVoice_ = _other.pSourceVoice_;
			//			massert(false && "AudioPlayer�̃R�s�[����");
			return *this;
		}

		/// <summary>
		/// AudioPlayerCP����Ă΂�鏉��������
		/// </summary>
		void Initialize() override;

		void SetAudio(const AudioHandle _hAudio);

		/// <summary>
		/// ���ʒ���
		/// </summary>
		void SetVolume(float volume);

		void Play();

	private:
		IXAudio2SourceVoice* pSourceVoice_{ nullptr };
		AudioClip* pAudioClip_{ nullptr };
	};

}
