#include "AudioClip.h"
#include "ReleaseUtility.h"
#include "MTAssert.h"
#include "WaveData.h"
#include "MTAssert.h"

#define DR_MP3_IMPLEMENTATION
#include "dr_libs/dr_mp3.h"

using mtbin::Utility::CompareId;

mtgb::AudioClip::AudioClip(std::string_view _filePath, ComPtr<IXAudio2> _pXAudio2)
	: pWaveData_ { nullptr }
	, pSourceVoice_ { nullptr }
{
	std::optional<mtbin::MemoryStream> ms = GetMemoryStream(_filePath);
	if (ms.has_value() == false)
		return;

	Load(ms.value(), _pXAudio2);
}

mtgb::AudioClip::~AudioClip()
{
	SAFE_DELETE(pWaveData_);
	if (pSourceVoice_ != nullptr)
	{
		pSourceVoice_->Stop(0);
		pSourceVoice_->SetOutputVoices(nullptr);
		pSourceVoice_->FlushSourceBuffers();
		pSourceVoice_->DestroyVoice();
		pSourceVoice_ = nullptr;
	}
}

void mtgb::AudioClip::Play(bool _loop)
{
	// 音声の再生を停止。そうしないとFlushSourceBuffersで消えない
	pSourceVoice_->Stop(0, 0);
	// 再生待ち中の音声を全て削除
	pSourceVoice_->FlushSourceBuffers();
	XAUDIO2_BUFFER buffer { .Flags		= XAUDIO2_END_OF_STREAM,
							.AudioBytes = static_cast<UINT32>(pWaveData_->bufferSize),
							.pAudioData = pWaveData_->pBuffer,
							.LoopCount	= 0 };

	if (_loop)
	{
		buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	}

	HRESULT hResult = pSourceVoice_->SubmitSourceBuffer(&buffer);
	if (FAILED(hResult))
	{
		massert(false && "ソースボイスにサブミット失敗");
		return;
	}

	pSourceVoice_->Start();
}

void mtgb::AudioClip::Stop() 
{
	pSourceVoice_->Stop(0, 0);
	pSourceVoice_->FlushSourceBuffers();
}

void mtgb::AudioClip::Load(mtbin::MemoryStream& _ms, ComPtr<IXAudio2> _pXAudio2)
{
	SAFE_DELETE(pWaveData_);
	pWaveData_ = new WaveData {};
	_ms.Seek(0);

	// 先頭4バイトを読み取る
	byte header[4];
	_ms.Read(header, sizeof(header), 4);

	// フォーマット判定
	if (CompareId<4>(header, "RIFF"))
	{
		// WAVよみこみ

		_ms.Seek(0);
		LoadWave(_ms);
	}
	else if (CompareId<3>(header, "ID3") || (header[0] == 0xFF && (header[1] == 0xFA || header[1] == 0xFB)))
	{
		_ms.Seek(0);
		LoadMp3(_ms);
	}
	else
	{
		massert(false && "対応していない音声フォーマットです @AudioClip::Load");
	}

	_pXAudio2->CreateSourceVoice(&pSourceVoice_, &pWaveData_->waveFormat);
}

void mtgb::AudioClip::LoadWave(mtbin::MemoryStream& _ms)
{
	// チャンク識別子は 4 byte
	static const size_t ID_SIZE { 4 };

	// チャンクヘッダ情報
	struct ChunkHeader
	{
		ChunkHeader()
			: id {}
			, size { 0 }
		{
		}

		byte id[ID_SIZE]; // チャンク識別子
		uint32_t size;	  // チャンクボディのサイズ
	};

	// RIFF チャンク
	ChunkHeader riff {};
	_ms.Read(riff.id, sizeof(riff.id), ID_SIZE);
	massert(CompareId<ID_SIZE>(riff.id, "RIFF") && "RIFF チャンクIdの不一致 @AudioClip::Load");
	riff.size = _ms.Read<uint32_t>();

	// WAVE チャンク
	byte wave[ID_SIZE] {};
	_ms.Read(wave, sizeof(wave), ID_SIZE);
	massert(CompareId<ID_SIZE>(wave, "WAVE") && "WAVE チャンクIdの不一致 @AudioClip::Load");

	// フォーマットチャンク
	ChunkHeader format {};
	_ms.Read(format.id, sizeof(format.id), ID_SIZE);
	massert(CompareId<ID_SIZE>(format.id, "fmt ") && "フォーマット チャンクIdの不一致 @AudioClip::Load");
	format.size = _ms.Read<uint32_t>();
	massert((format.size == 0x10) && "対応していないフォーマットサイズ @AudioClip::Load");
	_ms.Read(reinterpret_cast<byte*>(&pWaveData_->waveFormat), sizeof(WAVEFORMATEX), format.size);

	// wBitsPerSampleを設定
	pWaveData_->waveFormat.nBlockAlign = pWaveData_->waveFormat.nChannels * pWaveData_->waveFormat.wBitsPerSample / 8;
	pWaveData_->waveFormat.nAvgBytesPerSec = pWaveData_->waveFormat.nSamplesPerSec * pWaveData_->waveFormat.nBlockAlign;

	// dataチャンクを探す
	ChunkHeader header {};
	while (true)
	{
		_ms.Read(header.id, sizeof(header.id), ID_SIZE);
		header.size = _ms.Read<uint32_t>();

		if (CompareId<ID_SIZE>(header.id, "data"))
		{
			break; // dataチャンクに到達したら離脱
		}
		else
		{
			_ms.Seek(_ms.Current() + header.size); // dataチャンクでないなら読み飛ばし
		}
	}

	pWaveData_->bufferSize = header.size;
	pWaveData_->pBuffer	   = new byte[header.size] {};
	_ms.Read(pWaveData_->pBuffer, header.size, header.size);
}

void mtgb::AudioClip::LoadMp3(mtbin::MemoryStream& _ms)
{
	// MemoryStreamからmp3データを読み込む
	size_t size	   = _ms.GetLength();
	byte* mp3_data = new byte[size];
	_ms.Read(mp3_data, static_cast<int>(size), static_cast<int>(size));

	// drmp3のデコード初期化
	drmp3 config;
	if (!drmp3_init_memory(&config, mp3_data, size, NULL))
	{
		massert(false && "MP3の初期化に失敗 @AudioClip::LoadMp3");
		delete[] mp3_data;
	}

	// 総PCMサンプル数を取得
	drmp3_uint64 total_samples = drmp3_get_pcm_frame_count(&config);
	// バッファサイズを計算
	size_t pcm_sample_count = (size_t)(total_samples * config.channels);
	size_t pcm_bytes		= pcm_sample_count * sizeof(short);

	// PCM出力バッファ確保
	pWaveData_->bufferSize = pcm_bytes;
	pWaveData_->pBuffer	   = new byte[pcm_bytes];

	// PCMデータを読み込む
	drmp3_read_pcm_frames_s16(&config, total_samples, reinterpret_cast<short*>(pWaveData_->pBuffer));

	// WAVEフォーマット情報設定
	WAVEFORMATEX& wf   = pWaveData_->waveFormat;
	wf.wFormatTag	   = 1;
	wf.nChannels	   = static_cast<WORD>(config.channels);
	wf.nSamplesPerSec  = static_cast<DWORD>(config.sampleRate);
	wf.wBitsPerSample  = 16;
	wf.nBlockAlign	   = wf.nChannels * wf.wBitsPerSample / 8;
	wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;

	// 後始末
	drmp3_uninit(&config);
	delete[] mp3_data; // メモリリーク防止のために開放
}

float mtgb::AudioClip::GetTotalTimeSec() const
{
	if (pWaveData_ == nullptr)
	{
		return 0;
	}
	massert(pWaveData_->waveFormat.nAvgBytesPerSec != 0 && "0除算してしまいます。");
	// データサイズ / 1秒間あたりの読みバイト数 = 総再生時間
	return pWaveData_->bufferSize / static_cast<float>(pWaveData_->waveFormat.nAvgBytesPerSec);
}

void mtgb::AudioClip::SetVolume(float _volume) 
{
	pSourceVoice_->SetVolume(_volume);
}

std::optional<mtbin::MemoryStream> mtgb::AudioClip::GetMemoryStream(std::string_view _filePath)
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
		return std::nullopt;
	}

	BOOL succeed { FALSE };

	//  REF: https://learn.microsoft.com/ja-jp/windows/win32/api/fileapi/nf-fileapi-getfilesizeex
	LARGE_INTEGER fileSize {}; // ファイルサイズ格納用
	succeed = GetFileSizeEx(hFile, &fileSize);
	if (succeed == FALSE)
	{
		massert(false && "ファイルサイズ取得に失敗 @Audio::Load");
		return std::nullopt;
	}

	DWORD readedSize { 0 }; // 実際に読み取れたバイト数

	byte* pBuffer { new byte[fileSize.QuadPart] }; // バッファ動的確保

	succeed = ReadFile(hFile, pBuffer, static_cast<DWORD>(fileSize.QuadPart), &readedSize, NULL);
	if (succeed == FALSE || readedSize != fileSize.QuadPart)
	{
		massert(false && "ファイルの読み取りに失敗 @Audio::Load");
		delete[] pBuffer; // バッファ解放
		return std::nullopt;
	}

	CloseHandle(hFile); // ファイルを閉じる

	// メモリストリーム作成
	mtbin::MemoryStream ms { pBuffer, static_cast<size_t>(fileSize.QuadPart) };

	return ms;
}
