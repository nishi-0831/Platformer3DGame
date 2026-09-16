#include "Utility/CompileShaderUtility.h"
ID3D11InputLayout* CreateInputLayout(ID3D11Device* _pDevice, ID3DBlob* _pShaderBlob)
{
	ID3D11ShaderReflection* reflection = nullptr;
	HRESULT hr						   = D3DReflect(
		_pShaderBlob->GetBufferPointer(),
		_pShaderBlob->GetBufferSize(),
		IID_ID3D11ShaderReflection,
		reinterpret_cast<void**>(&reflection)
	);

	if (FAILED(hr))
		return nullptr;

	D3D11_SHADER_DESC shaderDesc;
	reflection->GetDesc(&shaderDesc);

	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDescs;

	for (UINT i = 0; i < shaderDesc.InputParameters; i++)
	{
		// シェーダーのシグネチャ
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		reflection->GetInputParameterDesc(i, &paramDesc);

		D3D11_INPUT_ELEMENT_DESC elementDesc { .SemanticName		 = paramDesc.SemanticName,
											   .SemanticIndex		 = paramDesc.SemanticIndex,
											   .InputSlot			 = 0,
											   .AlignedByteOffset	 = D3D11_APPEND_ALIGNED_ELEMENT,
											   .InputSlotClass		 = D3D11_INPUT_PER_VERTEX_DATA,
											   .InstanceDataStepRate = 0 };
		if (i == 0)
		{
			elementDesc.AlignedByteOffset = 0;
		}
		// レジスタ内でXYZWのどれが使用されているか

		// x
		if (paramDesc.Mask == 0b01)
		{
			switch (paramDesc.ComponentType)
			{
				case D3D_REGISTER_COMPONENT_UINT32 :
					elementDesc.Format = DXGI_FORMAT_R32_UINT;
					break;
				case D3D_REGISTER_COMPONENT_FLOAT32 :
					elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
					break;
				default :
					break;
			}
		}
		// xy
		else if (paramDesc.Mask == 0b11)
		{
			switch (paramDesc.ComponentType)
			{
				case D3D_REGISTER_COMPONENT_UINT32 :
					elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
					break;
				case D3D_REGISTER_COMPONENT_FLOAT32 :
					elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
					break;
				default :
					break;
			}
		}
		// xyz
		else if (paramDesc.Mask == 0b111)
		{
			switch (paramDesc.ComponentType)
			{
				case D3D_REGISTER_COMPONENT_UINT32 :
					elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
					break;
				case D3D_REGISTER_COMPONENT_FLOAT32 :
					elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
					break;
				default :
					break;
			}
		}
		// xyzw
		else if (paramDesc.Mask == 0b1111)
		{
			switch (paramDesc.ComponentType)
			{
				case D3D_REGISTER_COMPONENT_UINT32 :
					elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
					break;
				case D3D_REGISTER_COMPONENT_FLOAT32 :
					elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
					break;
				default :
					break;
			}
		}

		inputLayoutDescs.push_back(elementDesc);
	}

	ID3D11InputLayout* inputLayout = nullptr;
	hr							   = _pDevice->CreateInputLayout(
		inputLayoutDescs.data(),
		static_cast<UINT>(inputLayoutDescs.size()),
		_pShaderBlob->GetBufferPointer(),
		_pShaderBlob->GetBufferSize(),
		&inputLayout
	);
	if (FAILED(hr))
	{
		assert(false);
		return nullptr;
	}
	reflection->Release();
	return inputLayout;
}