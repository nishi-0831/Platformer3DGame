#include "Audio.h"
#include <xaudio2.h>
#include <xaudio2fx.h>
#include <fileapi.h>
#include "MTAssert.h"
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
		audio.Register("Shot", "Sound/Motion-Swish07-6.mp3");
		audio.Register("Saw", "Sound/Saw.mp3");
		audio.SetClipVolume("Saw", 0.5f);
		audio.SetClipVolume("FootstepMonsterRun", 0.5f);
		audio.SetClipVolume("TitleScene", 0.2f);
		audio.SetClipVolume("PlayScene", 0.1f);
		audio.SetClipVolume("GameOver", 0.5f);
		audio.SetClipVolume("GameClear", 0.5f);

		audio.SetMasterVolume(0.5f);
	}
} // namespace mtgb

mtgb::Audio::Audio()
	: pXAudio2_ { nullptr }
	, pMasteringVoice_ { nullptr }
	, pListenerTransform_ { nullptr }
	, attenuationVolumeDistance_ { 40.0f }
{
	listener_.pCone = nullptr;
}

mtgb::Audio::~Audio()
{
	for (auto itr = sourceMap_.begin(); itr != sourceMap_.end(); itr++)
	{
		for (auto source : itr->second)
		{
			source.pSourceVoice->DestroyVoice();
			source.pSourceVoice = nullptr;
		}
	}
	sourceMap_.clear();

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

	hResult = pXAudio2_->CreateMasteringVoice(&pMasteringVoice_, 1);
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
	hResult = X3DAudioInitialize(dwChannelMask, X3DAUDIO_SPEED_OF_SOUND, x3DInstance_);

	// 距離(0.0～1.0)、音量(0.0～1.0)
	volumePoints[0] = { 0.0f, 1.0f };
	volumePoints[1] = { 0.5f, 0.5f };
	volumePoints[2] = { 1.0f, 0.0f };

	volumeCurve_.pPoints	= volumePoints;
	volumeCurve_.PointCount = CONTROL_POINT_COUNT;

	RegisterAudios();
}

void mtgb::Audio::Update()
{
	UpdateListener();
	UpdateEmitter();
}

void mtgb::Audio::Register(std::string_view _soundName, std::string_view _filePath)
{
	WaveData* waveData = new WaveData(_filePath, pXAudio2_);
	std::vector<AudioSource> sources;
	X3DAUDIO_EMITTER pX3DEmitter;
	pX3DEmitter.pVolumeCurve		= &volumeCurve_;
	pX3DEmitter.CurveDistanceScaler = attenuationVolumeDistance_;
	pX3DEmitter.ChannelCount		= 2;
	pX3DEmitter.pLFECurve			= nullptr;

	for (int i = 0; i < SOURCE_VOICE_POOL_SIZE; i++)
	{
		IXAudio2SourceVoice* sourceVoice = nullptr;
		pXAudio2_->CreateSourceVoice(&sourceVoice, &waveData->waveFormat);

		sources.emplace_back(AudioSource { AudioEmitter { pX3DEmitter, INVALID_ENTITY }, sourceVoice });
	}
	waveDataMap_.emplace(_soundName, *waveData);
	sourceMap_.emplace(_soundName, sources);
}

void mtgb::Audio::SetMasterVolume(float _volume)
{
	pMasteringVoice_->SetVolume(_volume);
}

void mtgb::Audio::SetListenerEntityId(EntityId _id)
{
	pListenerTransform_ = &(Game::System<TransformCP>().Get(_id));
}

void mtgb::Audio::SetEmitter(EntityId _id, std::string_view _soundName, int _handle)
{
	auto itr = sourceMap_.find(_soundName);
	if (itr == sourceMap_.end())
		return;

	std::vector<AudioSource>& sources = itr->second;
	if (_handle < 0 || _handle >= sources.size())
		return;
	sources[_handle].emitter.emitterId = _id;
}

void mtgb::Audio::UpdateEmitter()
{
	if (pListenerTransform_ == nullptr)
		return;
	XAUDIO2_VOICE_DETAILS voiceDetails;
	pMasteringVoice_->GetVoiceDetails(&voiceDetails);

	for (auto& [name, sources] : sourceMap_)
	{
		for (auto source : sources)
		{
			AudioEmitter& emitter = source.emitter;
			if (emitter.emitterId == INVALID_ENTITY)
				continue;

			WaveData* pWaveData = &waveDataMap_.find(name)->second;

			X3DAUDIO_DSP_SETTINGS dspSettings;
			// 出力先となるマスターボイスのチャネル数
			dspSettings.DstChannelCount = voiceDetails.InputChannels;
			// ソースとなるクリップのチャネル数
			dspSettings.SrcChannelCount = pWaveData->waveFormat.nChannels;

			std::vector<FLOAT32> mat(dspSettings.SrcChannelCount * dspSettings.DstChannelCount);
			dspSettings.pMatrixCoefficients = mat.data();

			Transform* pEmitterTransform = nullptr;
			Game::System<TransformCP>().TryGet(pEmitterTransform, emitter.emitterId);
			if (pEmitterTransform == nullptr)
			{
				continue;
			}

			X3DAUDIO_EMITTER& pX3DEmitter = emitter.x3DEmitter;
			pX3DEmitter.Position		  = pEmitterTransform->GetWorldPosition();
			pX3DEmitter.OrientFront		  = pEmitterTransform->Forward();
			pX3DEmitter.OrientTop		  = pEmitterTransform->Up();
			// エミッターの座標を原点として、各チャネルが広がる半径
			pX3DEmitter.ChannelRadius = 1.0f;
			// エミッターの前方ベクトルを基準とした角度
			// 左右に分けるために、-π/2とπ/2を設定
			std::vector<FLOAT32> channelAzimuths = { -X3DAUDIO_2PI / 4.0f, X3DAUDIO_2PI / 4.0f };
			pX3DEmitter.pChannelAzimuths		 = channelAzimuths.data();

			X3DAudioCalculate(x3DInstance_, &listener_, &emitter.x3DEmitter, X3DAUDIO_CALCULATE_MATRIX, &dspSettings);
			source.pSourceVoice->SetOutputMatrix(
				pMasteringVoice_,
				dspSettings.SrcChannelCount,
				dspSettings.DstChannelCount,
				dspSettings.pMatrixCoefficients
			);
		}
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

void mtgb::Audio::SubmitSourceVoice(IXAudio2SourceVoice* _pSrcVoice, WaveData* _pWaveData, bool _isLoop)
{
	// 音声の再生を停止。そうしないとFlushSourceBuffersで消えない
	_pSrcVoice->Stop(0, 0);
	// 再生待ち中の音声を全て削除
	_pSrcVoice->FlushSourceBuffers();
	XAUDIO2_BUFFER buffer { .Flags		= XAUDIO2_END_OF_STREAM,
							.AudioBytes = static_cast<UINT32>(_pWaveData->bufferSize),
							.pAudioData = _pWaveData->pBuffer,
							.LoopCount	= 0 };

	if (_isLoop)
	{
		buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	}

	HRESULT hResult = _pSrcVoice->SubmitSourceBuffer(&buffer);
	if (FAILED(hResult))
	{
		massert(false && "ソースボイスにサブミット失敗");
		return;
	}

	_pSrcVoice->Start();
}

int mtgb::Audio::Play(std::string_view _soundName, bool _loop)
{
	auto itr = sourceMap_.find(_soundName);

	if (itr == sourceMap_.end())
		return -1;
	WaveData* waveData				 = &waveDataMap_.find(_soundName)->second;
	std::vector<AudioSource>& voices = itr->second;
	UINT complete					 = 0;
	for (int i = 0; i < voices.size(); i++)
	{
		XAUDIO2_VOICE_STATE state;
		voices[i].pSourceVoice->GetState(&state);

		if (state.BuffersQueued == complete)
		{
			// 再生処理
			SubmitSourceVoice(voices[i].pSourceVoice, waveData, _loop);
			return i;
		}
	}

	IXAudio2SourceVoice* bestVoice = nullptr;
	int min						   = INT_MAX;
	int idx						   = -1;
	for (int i = 0; i < voices.size(); i++)
	{
		XAUDIO2_VOICE_STATE state;
		voices[i].pSourceVoice->GetState(&state);

		int totalSample = waveData->bufferSize / waveData->waveFormat.nBlockAlign;
		int remaining	= totalSample - state.SamplesPlayed;
		if (remaining < min)
		{
			bestVoice = voices[i].pSourceVoice;
			idx		  = i;
			min		  = remaining;
		}
	}
	if (bestVoice)
	{
		SubmitSourceVoice(bestVoice, waveData, _loop);
	}
	return idx;
}

void mtgb::Audio::Stop(std::string_view _soundName, int _handle)
{
	auto itr = sourceMap_.find(_soundName);

	if (itr == sourceMap_.end())
		return;
	std::vector<AudioSource>& voices = itr->second;
	if (_handle < 0 || _handle >= voices.size())
		return;

	voices[_handle].pSourceVoice->Stop(0, 0);
	voices[_handle].pSourceVoice->FlushSourceBuffers();
}

void mtgb::Audio::Stop(std::string_view _soundName)
{
	auto itr = sourceMap_.find(_soundName);

	if (itr == sourceMap_.end())
		return;
	std::vector<AudioSource>& voices = itr->second;
	for (auto source : voices)
	{
		source.pSourceVoice->Stop(0, 0);
		source.pSourceVoice->FlushSourceBuffers();
	}
}

void mtgb::Audio::StopAll()
{
	for (auto itr = sourceMap_.begin(); itr != sourceMap_.end(); itr++)
	{
		for (auto voice : itr->second)
		{
			voice.pSourceVoice->Stop(0, 0);
			voice.pSourceVoice->FlushSourceBuffers();
		}
	}
}

void mtgb::Audio::SetClipVolume(std::string_view _soundName, float _volume)
{

	auto itr = sourceMap_.find(_soundName);
	if (itr == sourceMap_.end())
		return;

	for (auto source : itr->second)
	{
		source.pSourceVoice->SetVolume(_volume);
	}
}
