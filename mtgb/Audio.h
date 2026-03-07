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
		void Play(std::string_view _soundName);

	  private:
		/// <summary>
		/// 音声ファイルをロードする
		/// </summary>
		/// <param name="_filePath">ファイル名</param>
		/// <returns>音声クリップ</returns>
		AudioClip* Load(std::string_view _filePath);

	  private:
		ComPtr<IXAudio2> pXAudio2_;				  // XAudio2のインタフェース
		IXAudio2MasteringVoice* pMasteringVoice_; // 主ボイス
		std::unordered_map<std::string, AudioClip*, TransparentStringHash, TransparentStringEq> audioClipMap_;
	};
} // namespace mtgb
