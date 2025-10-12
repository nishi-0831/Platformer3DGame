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


mtgb::Audio::Audio() :
	pXAudio2_{ nullptr },
	pMasteringVoice_{ nullptr }
{
}

mtgb::Audio::~Audio()
{
	if (pMasteringVoice_ != nullptr)
	{
		pMasteringVoice_->DestroyVoice();
		pMasteringVoice_ = nullptr;
	}
	
}

void mtgb::Audio::Initialize()
{
	HRESULT hResult{};

	hResult = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	massert(SUCCEEDED(hResult)  // COM�̏������ɐ���
		&& "COM�̏������Ɏ��s @Audio::Initialize");

	hResult = XAudio2Create(&pXAudio2_);
	massert(SUCCEEDED(hResult)  // XAudio2�̍쐬�ɐ���
		&& "XAudio2�̍쐬�Ɏ��s @Audio::Initialize");

	hResult = pXAudio2_->CreateMasteringVoice(&pMasteringVoice_);
	massert(SUCCEEDED(hResult)  // MasteringVoice�̍쐬�ɐ���
		&& "MasteringVoice�̍쐬�Ɏ��s @Audio::Initialize");
}

void mtgb::Audio::Update()
{
	if (pOneShotQueue_.size() <= 0)
	{
		return;  // �L���[����Ȃ��A
	}

	//  REF: CPU��Sleep�v���Z�X

	// �Đ��I�����f�L���[����
	auto itr = pOneShotQueue_.begin();
	while (true)
	{
		(*itr)->timeLeft -= Time::DeltaTimeF();

		float diff{ (*itr)->timeLeft };  // ����
		if (diff > 0)  // ������0���傫�� = �Đ���
		{
			break;  // �Đ��I�����Ă��Ȃ����ߗ��E
		}
		else  // ������0�ȉ� = �Đ��I��
		{
			(*itr)->Release();
			//delete (*itr);  // �������
			itr = pOneShotQueue_.erase(pOneShotQueue_.begin());  // ����
			if (itr == pOneShotQueue_.end())
			{
				break;  // �㑱���Ȃ� = �L���[����Ȃ痣�E
			}
			(*itr)->timeLeft += diff;  // �������㑱�ɔ��f
			continue;  // �㑱���I�����Ă���\�������邽�ߌp��
		}
	}
}

mtgb::AudioClip* mtgb::Audio::GetAudioClip(const AudioHandle _hAudio)
{
	massert((pAudioClips_.count(_hAudio) > 0)
		&& "�w�肳�ꂽ�n���h���͖���/�܂����[�h����Ă��Ȃ�");

	return pAudioClips_[_hAudio];
}

void mtgb::Audio::CreateSourceVoice(IXAudio2SourceVoice** ppSourceVoice, const WaveData* _pWaveData)
{
	HRESULT hResult{};

	hResult = pXAudio2_->CreateSourceVoice(ppSourceVoice, &_pWaveData->waveFormat);
	massert(SUCCEEDED(hResult)  // SourceVoice�쐬�ɐ���
		&& "SourceVoice�쐬�Ɏ��s @Audio::SetSourceVoice");
}

mtgb::AudioHandle mtgb::Audio::Load(const std::string& _fileName)
{
	handleCounter_++;
	pAudioClips_.insert({ handleCounter_, new AudioClip{} });

	//  REF: https://learn.microsoft.com/ja-jp/windows/win32/api/fileapi/nf-fileapi-createfilea
	HANDLE hFile = CreateFile(
		_fileName.c_str(),      // �t�@�C����
		GENERIC_READ,           // �ǂݎ��܂���[
		FILE_SHARE_READ,        // Close�����܂ŁA���̃A�v���̓t�@�C���̓ǂݎ�肾�����Ă�����[
		nullptr,                // �Z�L�����e�B�����p�̍\���̃|�C���^���w��
		OPEN_EXISTING,          // �J�� - �t�@�C�������������玸�s
		FILE_ATTRIBUTE_NORMAL,  // ���ʂ̃t�@�C������
		NULL);                  // �����̃t�@�C�����J���ꍇ�͊֌W�Ȃ����

	if (hFile == INVALID_HANDLE_VALUE)
	{
		massert(false && "�t�@�C��Open�Ɏ��s @Audio::Load");
		return INVALID_HANDLE;
	}

	BOOL succeed{ FALSE };

	//  REF: https://learn.microsoft.com/ja-jp/windows/win32/api/fileapi/nf-fileapi-getfilesizeex
	LARGE_INTEGER fileSize{};  // �t�@�C���T�C�Y�i�[�p
	succeed = GetFileSizeEx(hFile, &fileSize);
	if (succeed == FALSE)
	{
		massert(false && "�t�@�C���T�C�Y�擾�Ɏ��s @Audio::Load");
		return INVALID_HANDLE;
	}

	DWORD readedSize{ 0 };  // ���ۂɓǂݎ�ꂽ�o�C�g��

	byte* pBuffer{ new byte[fileSize.QuadPart] };  // �o�b�t�@���I�m��

	succeed = ReadFile(hFile, pBuffer, static_cast<DWORD>(fileSize.QuadPart), &readedSize, NULL);
	if (succeed == FALSE || readedSize != fileSize.QuadPart)
	{
		massert(false && "�t�@�C���̓ǂݎ��Ɏ��s @Audio::Load");
		delete[] pBuffer;  // �o�b�t�@���
		return INVALID_HANDLE;
	}

	CloseHandle(hFile);  // �t�@�C�������

	// �����ǂݍ���
	LoadAudioSource(handleCounter_, pBuffer, fileSize.QuadPart);

	delete[] pBuffer;  // �o�b�t�@���

	return handleCounter_;
}

void mtgb::Audio::PlayOneShotBuffer(byte* pBuffer, const size_t _bufferSize)
{
	AudioOneShot* oneShot{ new AudioOneShot{} };

	mtbin::MemoryStream ms{ pBuffer, _bufferSize };
	oneShot->pAudioClip->Load(ms);

	Game::System<Audio>().CreateSourceVoice(
		&oneShot->pSourceVoice,
		oneShot->pAudioClip->pWaveData_);

	oneShot->timeLeft = static_cast<float>(oneShot->pAudioClip->GetTotalTimeSec());

	EnqueueOneShot(oneShot);  // ��������L���[�ɒǉ�

	const XAUDIO2_BUFFER BUFFER
	{
		.Flags = XAUDIO2_END_OF_STREAM,
		.AudioBytes = static_cast<UINT32>(oneShot->pAudioClip->pWaveData_->bufferSize),
		.pAudioData = oneShot->pAudioClip->pWaveData_->pBuffer,
		.LoopCount = 0,
	};

	HRESULT hResult{};
	hResult = oneShot->pSourceVoice->SubmitSourceBuffer(&BUFFER);

	massert(SUCCEEDED(hResult)  // SourceBuffer�̃Z�b�g�ɐ���
		&& "SourceBuffer�̃Z�b�g�Ɏ��s @Audio::PlayOneShotBuffer");

	oneShot->pSourceVoice->Start();  // �Đ�
}

void mtgb::Audio::PlayOneShotFile(const std::string& _fileName)
{
	//  REF: https://learn.microsoft.com/ja-jp/windows/win32/api/fileapi/nf-fileapi-createfilea
	HANDLE hFile = CreateFile(
		_fileName.c_str(),      // �t�@�C����
		GENERIC_READ,           // �ǂݎ��܂���[
		FILE_SHARE_READ,        // Close�����܂ŁA���̃A�v���̓t�@�C���̓ǂݎ�肾�����Ă�����[
		nullptr,                // �Z�L�����e�B�����p�̍\���̃|�C���^���w��
		OPEN_EXISTING,          // �J�� - �t�@�C�������������玸�s
		FILE_ATTRIBUTE_NORMAL,  // ���ʂ̃t�@�C������
		NULL);                  // �����̃t�@�C�����J���ꍇ�͊֌W�Ȃ����

	if (hFile == INVALID_HANDLE_VALUE)
	{
		massert(false && "�t�@�C��Open�Ɏ��s @Audio::Load");
		return;
	}

	BOOL succeed{ FALSE };

	//  REF: https://learn.microsoft.com/ja-jp/windows/win32/api/fileapi/nf-fileapi-getfilesizeex
	LARGE_INTEGER fileSize{};  // �t�@�C���T�C�Y�i�[�p
	succeed = GetFileSizeEx(hFile, &fileSize);
	if (succeed == FALSE)
	{
		massert(false && "�t�@�C���T�C�Y�擾�Ɏ��s @Audio::Load");
		return;
	}

	DWORD readedSize{ 0 };  // ���ۂɓǂݎ�ꂽ�o�C�g��

	byte* pBuffer{ new byte[fileSize.QuadPart] };  // �o�b�t�@���I�m��

	succeed = ReadFile(hFile, pBuffer, static_cast<DWORD>(fileSize.QuadPart), &readedSize, NULL);
	if (succeed == FALSE || readedSize != fileSize.QuadPart)
	{
		massert(false && "�t�@�C���̓ǂݎ��Ɏ��s @Audio::Load");
		delete[] pBuffer;  // �o�b�t�@���
		return;
	}

	CloseHandle(hFile);  // �t�@�C�������

	// �����Đ�
	PlayOneShotBuffer(pBuffer, fileSize.QuadPart);

	delete[] pBuffer;  // �o�b�t�@���
}

void mtgb::Audio::Clear()
{
	for (auto& oneShot : pOneShotQueue_)
	{
		oneShot->pSourceVoice->ExitLoop();
		oneShot->pSourceVoice->Stop();
		oneShot->Release();
		delete oneShot;
	}
	pOneShotQueue_.clear();
	for (auto& audioClip : pAudioClips_)
	{
		SAFE_DELETE(audioClip.second);
	}
	pAudioClips_.clear();
}

void mtgb::Audio::LoadAudioSource(const AudioHandle _hAudio, byte* pBuffer, const size_t _bufferSize)
{
	mtbin::MemoryStream ms{ pBuffer, _bufferSize };
	pAudioClips_[_hAudio]->Load(ms);
}

void mtgb::Audio::EnqueueOneShot(AudioOneShot* _pOneShot)
{
	float lefter{ _pOneShot->timeLeft };  // ���Z�p
	float righter{ 0 };                   // ���Z�p
	// �K�؂ȑ}���|�C���g��������
	for (auto itr = pOneShotQueue_.begin(); itr != pOneShotQueue_.end(); itr++)
	{
		if (lefter <= righter + (*itr)->timeLeft)
		{
			_pOneShot->timeLeft = lefter - righter;
			itr = pOneShotQueue_.insert(itr, _pOneShot);
			itr++;
			(*itr)->timeLeft -= _pOneShot->timeLeft;
			return;
		}
		righter += (*itr)->timeLeft;
	}
	// ������Ȃ������疖�[�ɒǉ�
	pOneShotQueue_.push_back(_pOneShot);
}

mtgb::AudioHandle mtgb::Audio::handleCounter_{ mtgb::INVALID_HANDLE };
std::map<mtgb::AudioHandle, mtgb::AudioClip*> mtgb::Audio::pAudioClips_{};
std::list<mtgb::AudioOneShot*> mtgb::Audio::pOneShotQueue_{};
