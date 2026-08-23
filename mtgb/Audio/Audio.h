#pragma once
#include "Core/ISystem.h"
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
	struct AudioSource
	{
		AudioEmitter emitter;
		IXAudio2SourceVoice* pSourceVoice;
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
		/// <para> 登録されていない識別子が渡された場合は何も起こらない </para>
		/// </summary>
		/// <param name="_soundName">Audio::Registerで登録した識別子</param>
		/// <param name="_loop">ループをするか否か</param>
		/// <returns>再生する音声のハンドルを返す</returns>
		int Play(std::string_view _soundName, bool _loop = false);

		/// <summary>
		/// 指定したソース音声の再生を停止する
		/// </summary>
		/// <param name="_soundName">Audio::Registerで登録した識別子</param>
		/// <param name="_handle">Audio::Playを呼んだ際に返されたハンドル</param>
		void Stop(std::string_view _soundName, int _handle);
		/// <summary>
		/// 指定した名前の音声の再生を全て停止する
		/// </summary>
		/// <param name="_soundName">Audio::Registerで登録した識別子</param>
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

		/// <summary>
		/// リスナーを設定する
		/// EntityIdに対応するTransformを利用する
		/// </summary>
		/// <param name="_id"></param>
		void SetListenerEntityId(EntityId _id);
		/// <summary>
		/// エミッターを設定する
		/// EntityIdに対応するTransformを利用する
		/// </summary>
		/// <param name="_id">エミッターの座標となるEntityId</param>
		/// <param name="_soundName">音声の名前</param>
		/// <param name="_handle">Audio::Playを呼んだ際に返されたハンドル</param>
		void SetEmitter(EntityId _id, std::string_view _soundName, int _handle);

	  private:
		void UpdateEmitter();
		void UpdateListener();
		void SubmitSourceVoice(IXAudio2SourceVoice* _pSrcVoice, WaveData* _pWaveData, bool _isLoop);

	  private:
		ComPtr<IXAudio2> pXAudio2_; // XAudio2のインタフェース
		X3DAUDIO_HANDLE x3DInstance_;
		IXAudio2MasteringVoice* pMasteringVoice_; // 主ボイス
		std::unordered_map<std::string, WaveData, TransparentStringHash, TransparentStringEq> waveDataMap_;
		std::unordered_map<std::string, std::vector<AudioSource>, TransparentStringHash, TransparentStringEq>
			sourceMap_;
		X3DAUDIO_LISTENER listener_;
		// 音量減衰の度合い。曲線で定義する
		X3DAUDIO_DISTANCE_CURVE volumeCurve_;
		Transform* pListenerTransform_;
		// 一種類の音声ごとの、ソースボイスの容量
		inline static constexpr int SOURCE_VOICE_POOL_SIZE { 3 };
		// 音量の減衰計算に使う距離。この値を1.0として音量を減衰させる
		inline static constexpr int CONTROL_POINT_COUNT { 3 };
		float attenuationVolumeDistance_;
		float volumeControlPointCount_;
		X3DAUDIO_DISTANCE_CURVE_POINT volumePoints[CONTROL_POINT_COUNT];
	};
} // namespace mtgb
