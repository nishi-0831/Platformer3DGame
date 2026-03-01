#include "Audio.h"
#include <xaudio2.h>
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
		audio.Register("GameOver", "Sound/GameOverSound.mp3");
		audio.Register("GameClear", "Sound/ClearScene.mp3");
		audio.Register("Stomp", "Sound/Stomp.mp3");
		audio.Register("Jump", "Sound/Jump.mp3");
		audio.Register("ItemGet", "Sound/ItemGetSound.mp3");
	}
} // namespace mtgb

mtgb::Audio::Audio()
	: pXAudio2_{nullptr}
	, pMasteringVoice_{nullptr}
{
}

mtgb::Audio::~Audio()
{
	if (pMasteringVoice_ != nullptr)
	{
		pMasteringVoice_->DestroyVoice();
		pMasteringVoice_ = nullptr;
	}
	pXAudio2_.Reset();
}

void mtgb::Audio::Initialize()
{
	HRESULT hResult{};

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

	hResult = pXAudio2_->CreateMasteringVoice(&pMasteringVoice_);
	massert(
		SUCCEEDED(hResult) // MasteringVoiceの作成に成功
		&& "MasteringVoiceの作成に失敗 @Audio::Initialize"
	);

	RegisterAudios();
}

void mtgb::Audio::Update()
{
}

void mtgb::Audio::Register(std::string_view _soundName, std::string_view _filePath)
{
	// 登録済みの場合はreturn
	if (audioClipMap_.contains(_soundName))
		return;

	audioClipMap_.emplace(_soundName, Load(_filePath));
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