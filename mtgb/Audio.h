#pragma once
#include "ISystem.h"
#include "cmtgb.h"
#include <string>
#include <map>
#include <functional>


struct IXAudio2;
struct IXAudio2MasteringVoice;
struct IXAudio2SourceVoice;

namespace mtgb
{
	class AudioClip;
	struct WaveData;
	struct AudioOneShot;

	using AudioHandle = int32_t;

	class Audio : public ISystem
	{
	public:
		Audio();
		~Audio();

		void Initialize() override;
		void Update() override;

		/// <summary>
		/// ���[�h�ς݂�AudioClip���擾
		/// </summary>
		/// <param name="_hAudio">�����n���h��</param>
		/// <returns>AudioSource�̃|�C���^</returns>
		AudioClip* GetAudioClip(const AudioHandle _hAudio);

		/// <summary>
		/// SourceVoice���쐬����
		/// </summary>
		/// <param name="ppSourceVoice">�i�[�pSourceVoice�̃|�C���^�n��</param>
		/// <param name="_pWaveData">�t�H�[�}�b�g�p��WaveData</param>
		void CreateSourceVoice(IXAudio2SourceVoice** ppSourceVoice, const WaveData* _pWaveData);

	private:
		IXAudio2* pXAudio2_;                       // XAudio2�̃C���^�t�F�[�X
		IXAudio2MasteringVoice* pMasteringVoice_;  // ��{�C�X

	public:
		/// <summary>
		/// �����t�@�C�������[�h����
		/// </summary>
		/// <param name="_fileName">�t�@�C����</param>
		/// <returns>�����n���h��</returns>
		static AudioHandle Load(const std::string& _fileName);
		/// <summary>
		/// Wave�t�@�C���̃o�b�t�@�𒼍Đ�����
		/// </summary>
		/// <param name="pBuffer">Wave�t�@�C���̃o�b�t�@�|�C���^</param>
		/// <param name="_bufferSize">�o�b�t�@�T�C�Y</param>
		static void PlayOneShotBuffer(byte* pBuffer, const size_t _bufferSize);
		/// <summary>
		/// �����t�@�C���̃o�b�t�@�𒼍Đ�����
		/// </summary>
		/// <param name="_fileName">�t�@�C����</param>
		static void PlayOneShotFile(const std::string& _fileName);
		/// <summary>
		/// �Đ��L���[���N���A����
		/// </summary>
		static void Clear();

	private:
		/// <summary>
		/// �w��n���h���̉��������[�h����
		/// </summary>
		/// <param name="_hAudio">�����n���h��</param>
		/// <param name="pBuffer">���[�h����o�b�t�@</param>
		/// <param name="_bufferSize">���[�h����o�b�t�@�T�C�Y</param>
		static void LoadAudioSource(const AudioHandle _hAudio, byte* pBuffer, const size_t _bufferSize);
		/// <summary>
		/// <para>�P�������L���[�ɒǉ�</para>
		/// <para>CPU��Sleep�v���Z�X���Q�l</para>
		/// </summary>
		/// <param name="_oneShot">�ǉ�����P������</param>
		static void EnqueueOneShot(AudioOneShot* _pOneShot);

	private:
		static AudioHandle handleCounter_;                      // �n���h�����Z�p
		static std::map<AudioHandle, AudioClip*> pAudioClips_;  // �n���h����AudioClip�̑Ή��tmap
		static std::list<AudioOneShot*> pOneShotQueue_;         // �P�������̃L���[
	};
}
