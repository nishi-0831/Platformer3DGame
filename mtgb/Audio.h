#pragma once
#include "ISystem.h"
#include "cmtgb.h"
#include "StringComparators.h"
#include <string>
#include <map>
#include <functional>
#include <wrl/client.h>
struct IXAudio2;
struct IXAudio2MasteringVoice;
struct IXAudio2SourceVoice;
using Microsoft::WRL::ComPtr;
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
		/// SourceVoiceを作成する
		/// </summary>
		/// <param name="ppSourceVoice">格納用SourceVoiceのポインタ渡し</param>
		/// <param name="_pWaveData">フォーマット用のWaveData</param>
		void CreateSourceVoice(IXAudio2SourceVoice** _ppSourceVoice, const WaveData* _pWaveData);

	  private:
		ComPtr<IXAudio2> pXAudio2_;				  // XAudio2のインタフェース
		IXAudio2MasteringVoice* pMasteringVoice_; // 主ボイス

	  public:
		/// <summary>
		/// <para> 音声ファイルを登録する </para>
		/// <para> 引数(_soundName)に渡した名前が音声データの識別子になる</para>
		/// </summary>
		/// <param name="_soundName">音声の名前</param>
		/// <param name="_filePath">音声のファイル</param>
		static void Register(std::string_view _soundName, std::string_view _filePath);
		/// <summary>
		/// Waveファイルのバッファを直再生する
		/// </summary>
		/// <param name="pBuffer">Waveファイルのバッファポインタ</param>
		/// <param name="_bufferSize">バッファサイズ</param>
		static void PlayOneShotBuffer(byte* _pBuffer, const size_t _bufferSize);
		/// <summary>
		/// 音声ファイルのバッファを直再生する
		/// </summary>
		/// <param name="_fileName">ファイル名</param>
		static void PlayOneShotFile(std::string_view _fileName);
		/// <summary>
		/// 再生キューをクリアする
		/// </summary>
		static void Clear();

		static void Play(std::string_view _soundName);

	  private:
		/// <summary>
		/// 音声ファイルをロードする
		/// </summary>
		/// <param name="_filePath">ファイル名</param>
		/// <returns>音声クリップ</returns>
		static AudioClip* Load(std::string_view _filePath);
		/// <summary>
		/// <para>単発音声キューに追加</para>
		/// <para>CPUのSleepプロセスを参考</para>
		/// </summary>
		/// <param name="_oneShot">追加する単発音声</param>
		static void EnqueueOneShot(AudioOneShot* _pOneShot);

	  private:
		static AudioHandle handleCounter_;					  // ハンドラ加算用
		static std::map<AudioHandle, AudioClip*> audioClips_; // ハンドルとAudioClipの対応付map
		static std::list<AudioOneShot*> oneShotQueue_;		  // 単発音声のキュー
		static std::unordered_map<std::string, AudioClip*, TransparentStringHash, TransparentStringEq> audioClipMap_;
	};
} // namespace mtgb
