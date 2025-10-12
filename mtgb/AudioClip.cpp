#include "AudioClip.h"
#include "ReleaseUtility.h"
#include "cmtgb.h"
#include "WaveData.h"

#define DR_MP3_IMPLEMENTATION
#include "../dr_mp3.h"

using mtbin::Utility::CompareId;

mtgb::AudioClip::AudioClip() :
	pWaveData_{ nullptr }
{
}

mtgb::AudioClip::~AudioClip()
{
	SAFE_DELETE(pWaveData_);
}

void mtgb::AudioClip::Load(mtbin::MemoryStream& _ms)
{
	SAFE_DELETE(pWaveData_);
	pWaveData_ = new WaveData{};
	_ms.Seek(0);

	// �擪4�o�C�g��ǂݎ��
	byte header[4];
	_ms.Read(header, sizeof(header), 4);

	// �t�H�[�}�b�g����
	if (CompareId<4>(header, "RIFF"))
	{
		// WAV��݂��� 

		_ms.Seek(0);
		LoadWave(_ms, header);
	}
	else if (CompareId<3>(header, "ID3") || (header[0] == 0xFF && (header[1] % 0xE0) == 0xE0))
	{
		_ms.Seek(0);
		LoadMp3(_ms, header);
	}
	else
	{
		massert(false && "�Ή����Ă��Ȃ������t�H�[�}�b�g�ł� @AudioClip::Load");
	}
}

void mtgb::AudioClip::LoadWave(mtbin::MemoryStream& _ms, const byte* _first4)
{
	// �`�����N���ʎq�� 4 byte
	static const size_t ID_SIZE{ 4 };
	
	// �`�����N�w�b�_���
	struct ChunkHeader
	{
		ChunkHeader() :
			id{},
			size{ 0 }
		{}

		byte id[ID_SIZE];  // �`�����N���ʎq
		uint32_t size;  // �`�����N�{�f�B�̃T�C�Y
	};

	// RIFF �`�����N
	ChunkHeader riff{};
	_ms.Read(riff.id, sizeof(riff.id), ID_SIZE);
	massert(CompareId<ID_SIZE>(riff.id, "RIFF")
		&& "RIFF �`�����NId�̕s��v @AudioClip::Load");
	riff.size = _ms.Read<uint32_t>();

	// WAVE �`�����N
	byte wave[ID_SIZE]{};
	_ms.Read(wave, sizeof(wave), ID_SIZE);
	massert(CompareId<ID_SIZE>(wave, "WAVE")
		&& "WAVE �`�����NId�̕s��v @AudioClip::Load");

	// �t�H�[�}�b�g�`�����N
	ChunkHeader format{};
	_ms.Read(format.id, sizeof(format.id), ID_SIZE);
	massert(CompareId<ID_SIZE>(format.id, "fmt ")
		&& "�t�H�[�}�b�g �`�����NId�̕s��v @AudioClip::Load");
	format.size = _ms.Read<uint32_t>();
	massert((format.size == 0x10)
		&& "�Ή����Ă��Ȃ��t�H�[�}�b�g�T�C�Y @AudioClip::Load");
	_ms.Read(reinterpret_cast<byte*>(&pWaveData_->waveFormat), sizeof(WAVEFORMATEX), format.size);

	// wBitsPerSample��ݒ�
	/*pWaveData_->waveFormat.wBitsPerSample =
		pWaveData_->waveFormat.nBlockAlign * 8 / pWaveData_->waveFormat.nChannels;*/
	pWaveData_->waveFormat.nBlockAlign =
		pWaveData_->waveFormat.nChannels * pWaveData_->waveFormat.wBitsPerSample / 8;
	pWaveData_->waveFormat.nAvgBytesPerSec = pWaveData_->waveFormat.nSamplesPerSec * pWaveData_->waveFormat.nBlockAlign;

	// data�`�����N��T��
	ChunkHeader header{};
	while (true)
	{
		_ms.Read(header.id, sizeof(header.id), ID_SIZE);
		header.size = _ms.Read<uint32_t>();

		if (CompareId<ID_SIZE>(header.id, "data"))
		{
			break;  // data�`�����N�ɓ��B�����痣�E
		}
		else
		{
			_ms.Seek(_ms.Current() + header.size);  // data�`�����N�łȂ��Ȃ�ǂݔ�΂�
		}
	}

	pWaveData_->bufferSize = header.size;
	pWaveData_->pBuffer = new byte[header.size]{};
	_ms.Read(pWaveData_->pBuffer, header.size, header.size);
}

void mtgb::AudioClip::LoadMp3(mtbin::MemoryStream& _ms, const byte* _first4)
{
	// MemoryStream����mp3�f�[�^��ǂݍ���
	size_t size = _ms.GetLength();
	byte* mp3_data = new byte[size];
	_ms.Read(mp3_data, static_cast<int>(size), static_cast<int>(size));

	// drmp3�̃f�R�[�h������
	drmp3 config;
	if(!drmp3_init_memory(&config, mp3_data, size, NULL))
	{
		massert(false && "MP3�̏������Ɏ��s @AudioClip::LoadMp3");
		delete[] mp3_data;
	}

	// ��PCM�T���v�������擾
	drmp3_uint64 total_samples = drmp3_get_pcm_frame_count(&config);
	// �o�b�t�@�T�C�Y���v�Z
	size_t pcm_sample_count = (size_t)(total_samples * config.channels);
	size_t pcm_bytes = pcm_sample_count * sizeof(short);

	// PCM�o�̓o�b�t�@�m��
	pWaveData_->bufferSize = pcm_bytes;
	pWaveData_->pBuffer = new byte[pcm_bytes];

	// PCM�f�[�^��ǂݍ���
	drmp3_read_pcm_frames_s16(&config, total_samples, reinterpret_cast<short*>(pWaveData_->pBuffer));

	// WAVE�t�H�[�}�b�g���ݒ�
	WAVEFORMATEX& wf = pWaveData_->waveFormat;
	wf.wFormatTag = 1;
	wf.nChannels = static_cast<WORD>(config.channels);
	wf.nSamplesPerSec = static_cast<DWORD>(config.sampleRate);
	wf.wBitsPerSample = 16;
	wf.nBlockAlign = wf.nChannels * wf.wBitsPerSample / 8;
	wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;

	// ��n��
	drmp3_uninit(&config);
	delete[] mp3_data; // ���������[�N�h�~�̂��߂ɊJ��
}

float mtgb::AudioClip::GetTotalTimeSec() const
{
	if (pWaveData_ == nullptr)
	{
		return 0;
	}
	massert(pWaveData_->waveFormat.nAvgBytesPerSec != 0 && "0���Z���Ă��܂��܂��B");
	// �f�[�^�T�C�Y / 1�b�Ԃ�����̓ǂ݃o�C�g�� = ���Đ�����
	return pWaveData_->bufferSize / static_cast<float>(pWaveData_->waveFormat.nAvgBytesPerSec);
}
