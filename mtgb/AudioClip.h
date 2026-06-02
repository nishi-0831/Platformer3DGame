#pragma once
#include <xaudio2.h>
#include <wrl/client.h>

#include "MTBin/MemoryStream.h"
#include <string_view>
#include <optional>
typedef struct tWAVEFORMATEX WAVEFORMATEX;
using Microsoft::WRL::ComPtr;

namespace mtgb
{
	struct WaveData;

	/// <summary>
	/// <para>音声クリップ (=音声データ)</para>
	/// </summary>
	class AudioClip
	{

		friend class Audio;
	  public:
		AudioClip(std::string_view _filePath, ComPtr<IXAudio2> _pXAudio2);
		~AudioClip();

		/// <summary>
		/// <para> 音声を再生する </para>
		/// <para> 再生中の音声は停止、削除される </para>
		/// </summary>
		/// <param name="_loop"> ループ再生をするか否か </param>
		void Play(bool _loop);
		/// <summary>
		/// 音声の再生を停止する
		/// </summary>
		void Stop();
		/// <summary>
		/// 音声データの総再生時間(秒)を取得
		/// </summary>
		/// <returns></returns>
		float GetTotalTimeSec() const;
		/// <summary>
		/// 音量を設定
		/// </summary>
		/// <param name="_volume"></param>
		void SetVolume(float _volume);
	  private:
		AudioClip() = delete;
		std::optional<mtbin::MemoryStream> GetMemoryStream(std::string_view _filePath);
		/// <summary>
		/// 音声データをクリップにロードする
		/// </summary>
		/// <param name="_ms">音声データのメモリストリーム</param>
		/// <param name="_pXAudio2">XAudio2のインターフェース。ソースボイスの作成に使う</param>
		void Load(mtbin::MemoryStream& _ms, ComPtr<IXAudio2> _pXAudio2);
		/// <summary>
		/// WAV読み込み
		/// </summary>
		/// <param name="_ms">WAVのメモリストリーム</param>
		void LoadWave(mtbin::MemoryStream& _ms);

		/// <summary>
		/// mp3読み込み
		/// </summary>
		/// <param name="_ms">mp3のメモリストリーム</param>
		void LoadMp3(mtbin::MemoryStream& _ms);
		WaveData* pWaveData_; // Waveファイルのデータ
		IXAudio2SourceVoice* pSourceVoice_;
	};
} // namespace mtgb
