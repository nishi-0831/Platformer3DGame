#pragma once
#include <xaudio2.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace mtgb
{
	/// <summary>
	/// Waveファイルのデータ格納構造体
	/// </summary>
	struct WaveData
	{
		WaveData();
		WaveData(std::string_view _filePath, ComPtr<IXAudio2> _pXAudio2);
		~WaveData();

		WAVEFORMATEX waveFormat; // フォーマット情報
		byte* pBuffer;			 // データバッファポインタ配列
		size_t bufferSize;		 // データバッファサイズ
	};
} // namespace mtgb
