#pragma once
#include "ISystem.h"
#include "cmtgb.h"
#include "StringComparators.h"
#include <string>
#include <map>
#include <functional>
#include <wrl/client.h>
#include <x3daudio.h>

#pragma comment(lib, "xaudio2_8.lib")
struct IXAudio2;
struct IXAudio2MasteringVoice;
struct IXAudio2SourceVoice;
using Microsoft::WRL::ComPtr;
namespace mtgb
{
	class AudioClip;
	struct WaveData;

	struct AudioEmitter
	{
		X3DAUDIO_EMITTER x3DEmitter;
		EntityId emitterId;
	};

	class Audio : public ISystem
	{
	  public:
		Audio();
		~Audio();

		void Initialize() override;
		void Update() override;

		/// <summary>
		/// <para> 音声ファイルを登録する </para>
		/// <para> 引数(_soundName)に渡した名前が音声データの識別子になる</para>
		/// </summary>
		/// <param name="_soundName">音声の名前</param>
		/// <param name="_filePath">音声のファイル</param>
		void Register(std::string_view _soundName, std::string_view _filePath);

		/// <summary>
		/// <para> 音声を再生 </para>
		/// <para> 再生途中の音声は停止、削除される </para>
		/// <para> 登録されていない識別子が渡された場合は何も起こらない </para>
		/// </summary>
		/// <param name="_soundName">Registerで登録した識別子</param>
		/// <param name="_loop">ループをするか否か</param>
		void Play(std::string_view _soundName, bool _loop = false);
		/// <summary>
		/// 指定した音声の再生を停止する
		/// </summary>
		/// <param name="_soundName"></param>
		void Stop(std::string_view _soundName);
		/// <summary>
		/// 再生中の全音声を停止する
		/// </summary>
		void StopAll();
		/// <summary>
		/// <para> 指定した音声クリップの音量を設定 </para>
		/// <para> 0.0～1.0の範囲を指定</para>
		/// <para> 既定では1.0</para>
		/// </summary>
		/// <param name="_soundName"></param>
		/// <param name="_volume"></param>
		void SetClipVolume(std::string_view _soundName, float _volume);

		/// <summary>
		/// <para> 最終的な音量を設定 </para>
		/// <para> 0.0～1.0の範囲を指定</para>
		/// <para> 既定では1.0</para>
		/// </summary>
		/// <param name="_volume"></param>
		void SetMasterVolume(float _volume);

		void SetListenerEntityId(EntityId _id);
		void SetEmitter(EntityId _id, std::string_view _soundName);

	  private:
		/// <summary>
		/// 音声ファイルをロードする
		/// </summary>
		/// <param name="_filePath">ファイル名</param>
		/// <returns>音声クリップ</returns>
		AudioClip* Load(std::string_view _filePath);

		void UpdateEmitter();
		void UpdateListener();

	  private:
		ComPtr<IXAudio2> pXAudio2_; // XAudio2のインタフェース
		X3DAUDIO_HANDLE x3DInstance_;
		IXAudio2MasteringVoice* pMasteringVoice_; // 主ボイス
		std::unordered_map<std::string, AudioClip*, TransparentStringHash, TransparentStringEq> audioClipMap_;
		std::unordered_map<std::string, AudioEmitter, TransparentStringHash, TransparentStringEq> emitterMap_;
		X3DAUDIO_LISTENER listener_;
		// 音量減衰の度合い。曲線で定義する
		X3DAUDIO_DISTANCE_CURVE volumeCurve_;
		Transform* pListenerTransform_;
		// 音量の減衰計算に使う距離。この値を1.0として音量を減衰させる

		inline static constexpr int CONTROL_POINT_COUNT { 3 };
		float attenuationVolumeDistance_;
		float volumeControlPointCount_;
		X3DAUDIO_DISTANCE_CURVE_POINT volumePoints[CONTROL_POINT_COUNT];
	};
} // namespace mtgb
