#include "IShader.h"
#include <d3d11.h>
#include "SceneSystem.h"
#include "ReleaseUtility.h"
#include "CompileShaderUtility.h"
#include "HLSLInclude.h"
mtgb::IShader::IShader() {}

void mtgb::IShader::Bind(ID3D11DeviceContext* _pCtx)
{
	_pCtx->RSSetState(pRasterizerState_.Get());
	_pCtx->VSSetShader(pVertexShader_.Get(), nullptr, 0);
	_pCtx->PSSetShader(pPixelShader_.Get(), nullptr, 0);
	_pCtx->IASetInputLayout(pInputLayout_.Get());
}

ReflectiveConstantBuffer* mtgb::IShader::GetConstantBuffer(std::string_view _name)
{
	auto itr = cBufferMap_.find(_name);
	if (itr == cBufferMap_.end())
	{
		return nullptr;
	}
	else
	{
		return &itr->second;
	}
	return nullptr;
}

void mtgb::IShader::InitializeCommonGpuResources(ID3D11Device* _pDevice, std::wstring_view _fileName)
{
	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	// デバッグ情報を埋め込み、最適化をオフにする
	dwShaderFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	HLSLInclude hlslInclude {};
	HRESULT hResult {};

	// 項点シェーダのインタフェース
	ID3DBlob* pCompileVS { nullptr };
	// 頂点シェーダのコンパイル
	hResult = D3DCompileFromFile(
		_fileName.data(), // ファイルパス
		nullptr,		  // シェーダマクロの配列
		&hlslInclude,	  // インクルードするやつ
		"VS",			  // エントリポイントの関数名
		"vs_5_0",		  // シェーダのバージョン (オプションで付けるやつ)
		dwShaderFlags,	  // オプションフラグ1
		0,				  // オプションフラグ2
		&pCompileVS,	  // コンパイル済みコードへのアクセスインタフェース
		nullptr
	); // エラーメッセージ受信用 無し
	massert(
		SUCCEEDED(hResult) // 頂点シェーダのコンパイルに成功
		&& "頂点シェーダのコンパイルに失敗 @IShader::CompileShader"
	);

	pInputLayout_.Attach(CreateInputLayout(_pDevice, pCompileVS));
	hResult = _pDevice->CreateVertexShader(
		pCompileVS->GetBufferPointer(),
		pCompileVS->GetBufferSize(),
		nullptr,
		pVertexShader_.ReleaseAndGetAddressOf()
	);
	massert(
		SUCCEEDED(hResult) // 頂点シェーダの作成に成功
		&& "頂点シェーダの作成に失敗 @IShader::CompileShader"
	);

	// ピクセルシェーダのインタフェース
	ID3DBlob* pCompilePS { nullptr };

	// ピクセルシェーダのコンパイル
	hResult = D3DCompileFromFile(
		_fileName.data(), // ファイルパス
		nullptr,		  // シェーダマクロの配列
		&hlslInclude,	  // インクルードするやつ
		"PS",			  // エントリポイントの関数名
		"ps_5_0",		  // シェーダのバージョン (オプションで付けるやつ)
		dwShaderFlags,	  // オプションフラグ1
		0,				  // オプションフラグ2
		&pCompilePS,	  // コンパイル済みコードへのアクセスインタフェース
		nullptr
	); // エラーメッセージ受信用 無し

	massert(
		SUCCEEDED(hResult) // ピクセルシェーダのコンパイルに成功
		&& "ピクセルシェーダのコンパイルに失敗 @IShader::CompileShader"
	);

	// ピクセルシェーダを作成し、指定タイプのバンドルに格納する
	hResult = DirectX11Draw::pDevice_->CreatePixelShader(
		pCompilePS->GetBufferPointer(), // コンパイルされたバッファのポインタ
		pCompilePS->GetBufferSize(),	// バッファのサイズ
		nullptr,						// リンケージクラス: 無し
		pPixelShader_.ReleaseAndGetAddressOf()
	);

	massert(
		SUCCEEDED(hResult) // ピクセルシェーダの作成に成功
		&& "ピクセルシェーダの作成に失敗 @IShader::CompileShader"
	);

	// シェーダーのリフレクション情報を取得
	ID3D11ShaderReflection* reflection = nullptr;
	hResult							   = D3DReflect(
		   pCompileVS->GetBufferPointer(),
		   pCompileVS->GetBufferSize(),
		   IID_ID3D11ShaderReflection,
		   reinterpret_cast<void**>(&reflection)
	   );
	D3D11_SHADER_INPUT_BIND_DESC bindDesc;
	D3D11_SHADER_DESC desc;
	reflection->GetDesc(&desc);
	for (UINT i = 0; i < desc.ConstantBuffers; i++)
	{
		// 定数バッファのリフレクション情報
		ID3D11ShaderReflectionConstantBuffer* cbReflection = reflection->GetConstantBufferByIndex(i);
		D3D11_SHADER_BUFFER_DESC cbDesc;
		cbReflection->GetDesc(&cbDesc);
		reflection->GetResourceBindingDescByName(cbDesc.Name, &bindDesc);

		// 定数バッファ作成
		ReflectiveConstantBuffer cBuf;
		cBuf.Initialize(_pDevice, cbReflection, bindDesc.BindPoint);
		cBufferMap_[cbDesc.Name] = cBuf;
	}

	SAFE_RELEASE(pCompileVS);
	SAFE_RELEASE(pCompilePS);
}

void mtgb::IShader::Release()
{
	for (auto& [name, buffer] : cBufferMap_)
	{
		buffer.Release();
	}
	cBufferMap_.clear();
	pInputLayout_.Reset();
	pVertexShader_.Reset();
	pPixelShader_.Reset();
	pRasterizerState_.Reset();
}
