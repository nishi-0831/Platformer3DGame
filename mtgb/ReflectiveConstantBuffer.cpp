#include "ReflectiveConstantBuffer.h"

ReflectiveConstantBuffer::ReflectiveConstantBuffer()
	: pBuffer_ { nullptr }
{
}

bool ReflectiveConstantBuffer::Initialize(
	ID3D11Device* _pDevice,
	ID3D11ShaderReflectionConstantBuffer* _pCbReflection,
	int _bindSlot
)
{
	Release();
	D3D11_SHADER_BUFFER_DESC cbDesc;
	_pCbReflection->GetDesc(&cbDesc);
	bindSlot_	= _bindSlot;
	bufferSize_ = cbDesc.Size;
	localBuffer_.resize(bufferSize_, 0);
	bufferName_ = cbDesc.Name;
	for (UINT i = 0; i < cbDesc.Variables; i++)
	{
		ID3D11ShaderReflectionVariable* varReflection = _pCbReflection->GetVariableByIndex(i);
		D3D11_SHADER_VARIABLE_DESC varDesc;
		varReflection->GetDesc(&varDesc);

		ShaderVariableInfo info;
		info.offset = varDesc.StartOffset;
		info.size	= varDesc.Size;

		variableMap_[varDesc.Name] = info;
	}

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth		   = bufferSize_;
	bufferDesc.Usage			   = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags		   = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags	   = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags		   = 0;
	bufferDesc.StructureByteStride = 0;

	HRESULT hr = _pDevice->CreateBuffer(&bufferDesc, nullptr, &pBuffer_);
	assert(SUCCEEDED(hr));
	return SUCCEEDED(hr);
}

void ReflectiveConstantBuffer::Release() {}

void ReflectiveConstantBuffer::ApplyChanges(ID3D11DeviceContext* _pContext)
{
	D3D11_MAPPED_SUBRESOURCE pdata_;

	_pContext->Map(pBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata_);
	memcpy_s(pdata_.pData, pdata_.RowPitch, reinterpret_cast<void*>(localBuffer_.data()), bufferSize_);
	_pContext->Unmap(pBuffer_, 0);
}

void ReflectiveConstantBuffer::BindVS(ID3D11DeviceContext* _pContext)
{
	_pContext->VSSetConstantBuffers(bindSlot_, 1, &pBuffer_);
}

void ReflectiveConstantBuffer::BindPS(ID3D11DeviceContext* _pContext)
{
	_pContext->PSSetConstantBuffers(bindSlot_, 1, &pBuffer_);
}

int ReflectiveConstantBuffer::GetBindSlot()
{
	return bindSlot_;
}
