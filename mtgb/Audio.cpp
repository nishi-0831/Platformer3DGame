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
#include "Transform.h"
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
	, pListenerTransform_ {nullptr}
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

	hResult = pXAudio2_->CreateMasteringVoice(&pMasteringVoice_,1);
	massert(
		SUCCEEDED(hResult) // MasteringVoiceの作成に成功
		&& "MasteringVoiceの作成に失敗 @Audio::Initialize"
	);
	
	// 音量制限用のAPOを作成
	IUnknown* pLimiterAPO;
	hResult = XAudio2CreateVolumeMeter(&pLimiterAPO);
	massert(SUCCEEDED(hResult) && "VolumeMeterAPOの作成に失敗");

	XAUDIO2_EFFECT_DESCRIPTOR desc {
		.pEffect		= pLimiterAPO,
		.InitialState	= TRUE,
		.OutputChannels = 1, // モノラル
	};
	XAUDIO2_EFFECT_CHAIN chain { .EffectCount = 1, .pEffectDescriptors = &desc };
	hResult = pMasteringVoice_->SetEffectChain(&chain);
	massert(SUCCEEDED(hResult) && "SetEffectChainに失敗");

	DWORD dwChannelMask;
	pMasteringVoice_->GetChannelMask(&dwChannelMask);
	X3DAudioInitialize(dwChannelMask, X3DAUDIO_SPEED_OF_SOUND, x3DInstance_);

	RegisterAudios();
}

void mtgb::Audio::Update() 
{
	UpdateListener();
	UpdateEmitter();
}

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

void mtgb::Audio::SetListenerEntityId(EntityId _id) 
{
	pListenerTransform_ = &(Game::System<TransformCP>().Get(_id));
}

void mtgb::Audio::SetEmitter(EntityId _id, std::string_view _soundName) 
{
	auto itr = audioClipMap_.find(_soundName);
	if (itr == audioClipMap_.end())
		return;

	Transform& transform = Game::System<TransformCP>().Get(_id);
	X3DAUDIO_EMITTER* pEmitter = nullptr;
	auto emitterItr			   = emitterMap_.find(_soundName);
	if (emitterItr == emitterMap_.end())
	{
		pEmitter = &emitterItr->second;
	}
	else
	{
		pEmitter = &emitterMap_[std::string(_soundName)];
	}

	pEmitter->Position		   = transform.GetWorldPosition();
	pEmitter->OrientFront	   = transform.Forward();
	pEmitter->OrientTop		   = transform.Up();
}

mtgb::AudioClip* mtgb::Audio::Load(std::string_view _filePath)
{
	AudioClip* audioClip = new AudioClip(_filePath, pXAudio2_);

	return audioClip;
}

void mtgb::Audio::UpdateEmitter() 
{
	XAUDIO2_VOICE_DETAILS voiceDetails;
	pMasteringVoice_->GetVoiceDetails(&voiceDetails);

	X3DAUDIO_DSP_SETTINGS dspSettings;
	dspSettings.SrcChannelCount = voiceDetails.InputChannels;

	for (auto [name, emitter] : emitterMap_)
	{
		AudioClip* clip					= audioClipMap_[name];
		dspSettings.DstChannelCount		= clip->pWaveData_->waveFormat.nChannels;
		FLOAT32* mat					= new FLOAT32[dspSettings.DstChannelCount * dspSettings.SrcChannelCount];
		dspSettings.pMatrixCoefficients = mat;
		X3DAudioCalculate(x3DInstance_, &listener_, &emitter, X3DAUDIO_CALCULATE_MATRIX, &dspSettings);
		clip->pSourceVoice_
			->SetOutputMatrix(pMasteringVoice_, 1, dspSettings.DstChannelCount, dspSettings.pMatrixCoefficients);
		clip->pSourceVoice_->SetFrequencyRatio(dspSettings.DopplerFactor);
	}
}

void mtgb::Audio::UpdateListener() 
{
	if (pListenerTransform_ == nullptr)
		return;
	listener_.OrientFront = pListenerTransform_->Forward();
	listener_.OrientTop	  = pListenerTransform_->Up();
	listener_.Position	  = pListenerTransform_->GetWorldPosition();

	
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
