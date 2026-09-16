#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <string>
#include <unordered_map>
#include <vector>
#include "Utility/StringComparators.h"

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

// 変数のオフセットとサイズ情報を保持する構造体
struct ShaderVariableInfo
{
	// 定数バッファ先頭からのバイトオフセット
	UINT offset;
	// 変数のバイトサイズ
	UINT size;
};

/// <summary>
/// リフレクションされた定数バッファ。
/// バッファ名、変数名を指定して値を設定できる
/// </summary>
class ReflectiveConstantBuffer
{
  public:
	ReflectiveConstantBuffer();
	/// <summary>
	///
	/// </summary>
	/// <param name="_pDevice"></param>
	/// <param name="_pCbReflection">DirectX11にリフレクションされた定数バッファ</param>
	/// <param name="_bindSlot">レジスタ番号</param>
	/// <returns></returns>
	bool Initialize(ID3D11Device* _pDevice, ID3D11ShaderReflectionConstantBuffer* _pCbReflection, UINT _bindSlot);
	void Release();
	/// <summary>
	/// 変更した値をGPUに送る
	/// </summary>
	/// <param name="_pContext"></param>
	void ApplyChanges(ID3D11DeviceContext* _pContext);
	/// <summary>
	/// 頂点シェーダーを設定する
	/// </summary>
	/// <param name="_pContext"></param>
	void BindVS(ID3D11DeviceContext* _pContext);
	/// <summary>
	/// ピクセルシェーダーを設定する
	/// </summary>
	/// <param name="_pContext"></param>
	void BindPS(ID3D11DeviceContext* _pContext);

	/// <summary>
	/// バッファ内の変数に値を設定する
	/// 変数と_dataのサイズが一致している必要がある
	/// </summary>
	/// <typeparam name="T">送信する型</typeparam>
	/// <param name="_name">設定する変数名</param>
	/// <param name="_data">送信する値</param>
	/// <returns>設定に成功: true、失敗: false</returns>
	template <typename T> bool SetVariable(std::string_view _name, const T& _data);
	/// <summary>
	/// バッファに値を設定する
	/// バッファと_dataのサイズが一致している必要がある
	/// </summary>
	/// <typeparam name="T">送信する型</typeparam>
	/// <param name="_data">送信する値</param>
	/// <returns>設定に成功: true、失敗: false</returns>
	template <typename T> bool SetConstantBuffer(const T& _data);
	/// <summary>
	/// レジスタ番号を返す
	/// </summary>
	/// <returns></returns>
	UINT GetBindSlot();

  private:
	// 定数バッファ
	ID3D11Buffer* pBuffer_;
	// バッファの名前
	std::string bufferName_;
	// サイズ
	UINT bufferSize_;
	// レジスタ番号
	UINT bindSlot_;
	// GPUに送るデータのキャッシュ。SetVariableなどで設定して、ApplyChangeで送信する
	std::vector<uint8_t> localBuffer_;
	// バッファ内の変数へのマップ
	std::unordered_map<std::string, ShaderVariableInfo, TransparentStringHash, TransparentStringEq> variableMap_;
};

template <typename T> inline bool ReflectiveConstantBuffer::SetVariable(std::string_view _name, const T& _data)
{
	auto itr = variableMap_.find(_name);
	if (itr == variableMap_.end())
	{
		return false;
	}

	const ShaderVariableInfo& info = itr->second;

	if (sizeof(T) == info.size)
	{
		return false;
	}

	std::memcpy(localBuffer_.data() + info.offset, &_data, sizeof(T));

	return true;
}

template <typename T> inline bool ReflectiveConstantBuffer::SetConstantBuffer(const T& _data)
{
	size_t dataSize = sizeof(T);
	if (dataSize != bufferSize_)
	{
		return false;
	}

	std::memcpy(localBuffer_.data(), &_data, sizeof(T));

	return true;
}
