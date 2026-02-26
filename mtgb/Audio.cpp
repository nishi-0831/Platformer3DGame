#include "Audio.h"
#include <xaudio2.h>
#include <fileapi.h>
#include "MTAssert.h"
#include "AudioClip.h"
#include "MTBin/MemoryStream.h"
#include "ReleaseUtility.h"
#include "WaveData.h"
#include "GameTime.h"
#include "AudioOneShot.h"
#include "Debug.h"

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
}

void mtgb::Audio::Update()
{
	if (oneShotQueue_.size() <= 0)
	{
		return; // キューが空なら回帰
	}

	//  REF: CPUのSleepプロセス

	// 再生終了をデキューする
	auto itr = oneShotQueue_.begin();
	while (true)
	{
		(*itr)->timeLeft -= Time::DeltaTimeF();

		float diff{(*itr)->timeLeft}; // 差分
		if (diff > 0)				  // 差分が0より大きい = 再生中
		{
			break; // 再生終了していないため離脱
		}
		else // 差分が0以下 = 再生終了
		{
			(*itr)->Release();
			// delete (*itr);  // 解放する
			itr = oneShotQueue_.erase(oneShotQueue_.begin()); // 消す
			if (itr == oneShotQueue_.end())
			{
				break; // 後続がない = キューが空なら離脱
			}
			(*itr)->timeLeft += diff; // 差分を後続に反映
			continue;				  // 後続も終了している可能性があるため継続
		}
	}
}

void mtgb::Audio::CreateSourceVoice(IXAudio2SourceVoice** _ppSourceVoice, const WaveData* _pWaveData)
{
	HRESULT hResult{};

	hResult = pXAudio2_->CreateSourceVoice(_ppSourceVoice, &_pWaveData->waveFormat);
	massert(
		SUCCEEDED(hResult) // SourceVoice作成に成功
		&& "SourceVoice作成に失敗 @Audio::SetSourceVoice"
	);
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
	//  REF: https://learn.microsoft.com/ja-jp/windows/win32/api/fileapi/nf-fileapi-createfilea
	HANDLE hFile = CreateFile(
		_filePath.data(),	   // ファイル名
		GENERIC_READ,		   // 読み取りますよー
		FILE_SHARE_READ,	   // Closeされるまで、他のアプリはファイルの読み取りだけしていいよー
		nullptr,			   // セキュリティ属性用の構造体ポインタを指定
		OPEN_EXISTING,		   // 開く - ファイルが無かったら失敗
		FILE_ATTRIBUTE_NORMAL, // 普通のファイル属性
		NULL
	); // 既存のファイルを開く場合は関係ないやつ

	if (hFile == INVALID_HANDLE_VALUE)
	{
		massert(false && "ファイルOpenに失敗 @Audio::Load");
		return nullptr;
	}

	BOOL succeed{FALSE};

	//  REF: https://learn.microsoft.com/ja-jp/windows/win32/api/fileapi/nf-fileapi-getfilesizeex
	LARGE_INTEGER fileSize{}; // ファイルサイズ格納用
	succeed = GetFileSizeEx(hFile, &fileSize);
	if (succeed == FALSE)
	{
		massert(false && "ファイルサイズ取得に失敗 @Audio::Load");
		return nullptr;
	}

	DWORD readedSize{0}; // 実際に読み取れたバイト数

	byte* pBuffer{new byte[fileSize.QuadPart]}; // バッファ動的確保

	succeed = ReadFile(hFile, pBuffer, static_cast<DWORD>(fileSize.QuadPart), &readedSize, NULL);
	if (succeed == FALSE || readedSize != fileSize.QuadPart)
	{
		massert(false && "ファイルの読み取りに失敗 @Audio::Load");
		delete[] pBuffer; // バッファ解放
		return nullptr;
	}

	CloseHandle(hFile); // ファイルを閉じる

	// いざ読み込み
	mtbin::MemoryStream ms{pBuffer, static_cast<size_t>(fileSize.QuadPart)};
	AudioClip* audioClip = new AudioClip();
	audioClip->Load(ms);

	delete[] pBuffer; // バッファ解放

	return audioClip;
}

void mtgb::Audio::PlayOneShotBuffer(byte* _pBuffer, const size_t _bufferSize)
{
	AudioOneShot* oneShot{new AudioOneShot{}};

	mtbin::MemoryStream ms{_pBuffer, _bufferSize};
	oneShot->pAudioClip->Load(ms);

	Game::System<Audio>().CreateSourceVoice(&oneShot->pSourceVoice, oneShot->pAudioClip->pWaveData_);

	oneShot->timeLeft = static_cast<float>(oneShot->pAudioClip->GetTotalTimeSec());

	EnqueueOneShot(oneShot); // 自動解放キューに追加

	const XAUDIO2_BUFFER BUFFER{
		.Flags		= XAUDIO2_END_OF_STREAM,
		.AudioBytes = static_cast<UINT32>(oneShot->pAudioClip->pWaveData_->bufferSize),
		.pAudioData = oneShot->pAudioClip->pWaveData_->pBuffer,
		.LoopCount	= 0,
	};

	HRESULT hResult{};
	hResult = oneShot->pSourceVoice->SubmitSourceBuffer(&BUFFER);

	massert(
		SUCCEEDED(hResult) // SourceBufferのセットに成功
		&& "SourceBufferのセットに失敗 @Audio::PlayOneShotBuffer"
	);

	oneShot->pSourceVoice->Start(); // 再生
}

void mtgb::Audio::PlayOneShotFile(std::string_view _fileName)
{
	//  REF: https://learn.microsoft.com/ja-jp/windows/win32/api/fileapi/nf-fileapi-createfilea
	HANDLE hFile = CreateFile(
		_fileName.data(),	   // ファイル名
		GENERIC_READ,		   // 読み取りますよー
		FILE_SHARE_READ,	   // Closeされるまで、他のアプリはファイルの読み取りだけしていいよー
		nullptr,			   // セキュリティ属性用の構造体ポインタを指定
		OPEN_EXISTING,		   // 開く - ファイルが無かったら失敗
		FILE_ATTRIBUTE_NORMAL, // 普通のファイル属性
		NULL
	); // 既存のファイルを開く場合は関係ないやつ

	if (hFile == INVALID_HANDLE_VALUE)
	{
		massert(false && "ファイルOpenに失敗 @Audio::Load");
		return;
	}

	BOOL succeed{FALSE};

	//  REF: https://learn.microsoft.com/ja-jp/windows/win32/api/fileapi/nf-fileapi-getfilesizeex
	LARGE_INTEGER fileSize{}; // ファイルサイズ格納用
	succeed = GetFileSizeEx(hFile, &fileSize);
	if (succeed == FALSE)
	{
		massert(false && "ファイルサイズ取得に失敗 @Audio::Load");
		return;
	}

	DWORD readedSize{0}; // 実際に読み取れたバイト数

	byte* pBuffer{new byte[fileSize.QuadPart]}; // バッファ動的確保

	succeed = ReadFile(hFile, pBuffer, static_cast<DWORD>(fileSize.QuadPart), &readedSize, NULL);
	if (succeed == FALSE || readedSize != fileSize.QuadPart)
	{
		massert(false && "ファイルの読み取りに失敗 @Audio::Load");
		delete[] pBuffer; // バッファ解放
		return;
	}

	CloseHandle(hFile); // ファイルを閉じる

	// いざ再生
	PlayOneShotBuffer(pBuffer, fileSize.QuadPart);

	delete[] pBuffer; // バッファ解放
}

void mtgb::Audio::Clear()
{
	for (auto& oneShot : oneShotQueue_)
	{
		oneShot->pSourceVoice->ExitLoop();
		oneShot->pSourceVoice->Stop();
		oneShot->Release();
		delete oneShot;
	}
	oneShotQueue_.clear();
	for (auto& audioClip : audioClips_)
	{
		SAFE_DELETE(audioClip.second);
	}
	audioClips_.clear();
}

void mtgb::Audio::Play(std::string_view _soundName)
{
	auto itr = audioClipMap_.find(_soundName);
	if (itr == audioClipMap_.end())
		return;

	AudioClip* audioClip	  = (*itr).second;
	const WaveData* pWaveData = audioClip->pWaveData_;

	XAUDIO2_BUFFER buffer{
		.Flags		= XAUDIO2_END_OF_STREAM,
		.AudioBytes = static_cast<UINT32>(pWaveData->bufferSize),
		.pAudioData = pWaveData->pBuffer,
		.LoopCount	= 0
	};

	// HRESULT hResult = ;
}

void mtgb::Audio::EnqueueOneShot(AudioOneShot* _pOneShot)
{
	float lefter{_pOneShot->timeLeft}; // 減算用
	float righter{0};				   // 加算用
	// 適切な挿入ポイントを見つける
	for (auto itr = oneShotQueue_.begin(); itr != oneShotQueue_.end(); itr++)
	{
		if (lefter <= righter + (*itr)->timeLeft)
		{
			_pOneShot->timeLeft = lefter - righter;
			itr					= oneShotQueue_.insert(itr, _pOneShot);
			itr++;
			(*itr)->timeLeft -= _pOneShot->timeLeft;
			return;
		}
		righter += (*itr)->timeLeft;
	}
	// 見つからなかったら末端に追加
	oneShotQueue_.push_back(_pOneShot);
}

mtgb::AudioHandle mtgb::Audio::handleCounter_{mtgb::INVALID_HANDLE};
std::map<mtgb::AudioHandle, mtgb::AudioClip*> mtgb::Audio::audioClips_{};
std::list<mtgb::AudioOneShot*> mtgb::Audio::oneShotQueue_{};
