#include "Audio.h"
#include <xaudio2.h>
#include <xaudio2fx.h>
#include <fileapi.h>
#include "MTAssert.h"
#include "AudioClip.h"
#include "MTBin/MemoryStream.h"
#include "ReleaseUtility.h"
#include "WaveData.h"
#include "GameTime.h"
#include "Debug.h"

namespace mtgb
{
	/// <summary>
	///  ゲームで使用する音声を登録
	/// </summary>
	void RegisterAudios()
	{
		mtgb::Audio& audio = Game::System<Audio>();
		audio.Register("PlayerDeath", "Sound/GameOverSound.mp3");
		audio.Register("GameOver", "Sound/maou_game_jingle08.mp3");
		audio.Register("GameClear", "Sound/clear2.mp3");
		audio.Register("PlayScene", "Sound/kouyaoiku.mp3");
		audio.Register("TitleScene", "Sound/opening.mp3");
		audio.Register("GetTreasure", "Sound/ClearScene.mp3");
		audio.Register("Stomp", "Sound/Stomp.mp3");
		audio.Register("Jump", "Sound/Jump.mp3");
		audio.Register("ItemGet", "Sound/ItemGetSound.mp3");
		audio.Register("MoveCursor", "Sound/MoveCursor.mp3");
		audio.Register("MinerFootstep", "Sound/MinerFootstep.mp3");
		audio.Register("FootstepMonsterWalk", "Sound/FootstepMonsterWalk.mp3");
		audio.Register("FootstepMonsterRun", "Sound/FootstepMonsterRun.mp3");
		audio.SetClipVolume("FootstepMonsterRun", 0.5f);
		audio.SetMasterVolume(0.5f);
	}
} // namespace mtgb

mtgb::Audio::Audio()
	: pXAudio2_ { nullptr }
	, pMasteringVoice_ { nullptr }
{
}

mtgb::Audio::~Audio()
{
	for (auto itr = audioClipMap_.begin(); itr != audioClipMap_.end(); itr++)
	{
		SAFE_DELETE(itr->second);
	}
	audioClipMap_.clear();

	if (pMasteringVoice_ != nullptr)
	{
		pMasteringVoice_->DestroyVoice();
		pMasteringVoice_ = nullptr;
	}

	// IXAudio2の解放は最後
	pXAudio2_.Reset();
}

void mtgb::Audio::Initialize()
{
	HRESULT hResult {};

	hResult = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	massert(
		SUCCEEDED(hResult) // COMの初期化に成功
		&& "COMの初期化に失敗 @Audio::Initialize"
	);

	hResult = XAudio2Create(pXAudio2_.ReleaseAndGetAddressOf());
	massert(
		SUCCEEDED(hResult) // XAudio2の作成に成功
		&& "XAudio2の作成に失敗 @Audio::Initialize"
	);

	hResult = pXAudio2_->CreateMasteringVoice(&pMasteringVoice_,2);
	massert(
		SUCCEEDED(hResult) // MasteringVoiceの作成に成功
		&& "MasteringVoiceの作成に失敗 @Audio::Initialize"
	);
	
	XAUDIO2_VOICE_DETAILS voiceDetails;
	pMasteringVoice_->GetVoiceDetails(&voiceDetails);
	UINT channelCnt = voiceDetails.InputChannels;

	// 音量制限用のAPOを作成
	IUnknown* pLimiterAPO;
	hResult = XAudio2CreateVolumeMeter(&pLimiterAPO);
	massert(SUCCEEDED(hResult) && "VolumeMeterAPOの作成に失敗");

	XAUDIO2_EFFECT_DESCRIPTOR desc {
		.pEffect		= pLimiterAPO,
		.InitialState	= TRUE,
		.OutputChannels = 2, // ステレオ
	};
	XAUDIO2_EFFECT_CHAIN chain { .EffectCount = 1, .pEffectDescriptors = &desc };
	hResult = pMasteringVoice_->SetEffectChain(&chain);
	massert(SUCCEEDED(hResult) && "SetEffectChainに失敗");
	RegisterAudios();
}

void mtgb::Audio::Update() {}

void mtgb::Audio::Register(std::string_view _soundName, std::string_view _filePath)
{
	// 登録済みの場合はreturn
	if (audioClipMap_.contains(_soundName))
		return;

	audioClipMap_.emplace(_soundName, Load(_filePath));
}

void mtgb::Audio::SetMasterVolume(float _volume) 
{
	pMasteringVoice_->SetVolume(_volume);
}

mtgb::AudioClip* mtgb::Audio::Load(std::string_view _filePath)
{
	AudioClip* audioClip = new AudioClip(_filePath, pXAudio2_);

	return audioClip;
}

void mtgb::Audio::Play(std::string_view _soundName)
{
	auto itr = audioClipMap_.find(_soundName);
	if (itr == audioClipMap_.end())
		return;

	itr->second->Play();
}

void mtgb::Audio::SetClipVolume(std::string_view _soundName, float _volume) 
{
	auto itr = audioClipMap_.find(_soundName);
	if (itr == audioClipMap_.end())
		return;

	itr->second->SetVolume(_volume);
}
