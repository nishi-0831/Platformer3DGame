#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <string>
#include <unordered_map>
#include <vector>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

// 変数のオフセットとサイズ情報を保持する構造体
struct ShaderVariableInfo
{
	UINT offset; // 定数バッファ先頭からのバイトオフセット
	UINT size;	 // 変数のバイトサイズ
};

class ReflectiveConstantBuffer
{
  public:
	ReflectiveConstantBuffer();
	bool Initialize(ID3D11Device* _pDevice, ID3D11ShaderReflectionConstantBuffer* _pCbReflection, int _bindSlot);
	void Release();
	void ApplyChanges(ID3D11DeviceContext* _pContext);
	void BindVS(ID3D11DeviceContext* _pContext);
	void BindPS(ID3D11DeviceContext* _pContext);

	template <typename T> bool SetVariable(const std::string& _name, const T& _data);
	template <typename T> bool SetConstantBuffer(const T& _data);
	int GetBindSlot();

  private:
	ID3D11Buffer* pBuffer_;
	std::string bufferName_;
	int bufferSize_;
	int bindSlot_;
	std::vector<uint8_t> localBuffer_;
	std::unordered_map<std::string, ShaderVariableInfo> variableMap_;
};

template <typename T> inline bool ReflectiveConstantBuffer::SetVariable(const std::string& _name, const T& _data)
{
	auto itr = variableMap_.find(_name);
	if (itr == variableMap_.end())
	{
		return false;
	}

	const ShaderVariableInfo& info = itr->second;

	if (sizeof(T) > info.size)
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
