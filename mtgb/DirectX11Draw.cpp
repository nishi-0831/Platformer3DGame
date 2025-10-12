#include "DirectX11Draw.h"
#include <d3d11.h>
#include <dxgi1_2.h>
#include <DirectXMath.h>
#include "Texture2D.h"
#include "ReleaseUtility.h"
#include "IShader.h"
#include "DirectWrite.h"
#include "OBJ.h"
#include "Debug.h"

using namespace mtgb;

ComPtr<ID3D11Device> DirectX11Draw::pDevice_{ nullptr };  // �`����s�����߂̊��A���\�[�X�̍쐬�Ɏg��
ComPtr<ID3D11DeviceContext> DirectX11Draw::pContext_{ nullptr };
ComPtr<IDXGIDevice1> DirectX11Draw::pDXGIDevice_{ nullptr };
std::vector<ComPtr<IDXGIAdapter1>> DirectX11Draw::pDXGIAdapters_{};
std::vector<MonitorInfo> DirectX11Draw::monitorInfos_{};
ComPtr<IDXGIFactory2> DirectX11Draw::pDXGIFactory_{ nullptr };

IDXGISwapChain* DirectX11Draw::pSwapChain_{ nullptr };  // �_�u���o�b�t�@�����O������
ComPtr<ID3D11RenderTargetView> DirectX11Draw::pRenderTargetView_{ nullptr };  // �`���
ComPtr<IDXGISwapChain1> DirectX11Draw::pSwapChain1_{ nullptr };
ComPtr<ID3D11DepthStencilView> DirectX11Draw::pDepthStencilView_{ nullptr };  // �[�x�o�b�t�@
std::array<ComPtr<ID3D11DepthStencilState>, static_cast<int8_t>(BlendMode::Max)> DirectX11Draw::pDepthStencilState_{nullptr};  // �u�����h�ɂ��[�x�o�b�t�@�ւ̏������ݏ��
ComPtr<ID3D11Texture2D> DirectX11Draw::pDepthStencil_{ nullptr };  // �u�����h�̏��
std::array<ComPtr<ID3D11BlendState>, static_cast<int8_t>(BlendMode::Max)> DirectX11Draw::pBlendState_{ nullptr };  // �u�����h�̏��
ComPtr<ID3D11SamplerState> DirectX11Draw::pDefaultSamplerState_{ nullptr };
ShaderBundle DirectX11Draw::shaderBundle_[static_cast<int8_t>(ShaderType::Max)]{};  // �V�F�[�_�̃o���h��
Vector4 DirectX11Draw::backgroundColor_{ 0, 1, 0, 1 };

void mtgb::DirectX11Draw::SetShader(const ShaderType _type)
{
	const int INDEX{ static_cast<int>(_type) };
	pContext_->RSSetState(shaderBundle_[INDEX].pRasterizerState.Get());
	pContext_->VSSetShader(shaderBundle_[INDEX].pVertexShader.Get(), nullptr, 0);
	pContext_->PSSetShader(shaderBundle_[INDEX].pPixelShader.Get(), nullptr, 0);
	pContext_->IASetInputLayout(shaderBundle_[INDEX].pVertexLayout.Get());
}

void mtgb::DirectX11Draw::SetBlendMode(const BlendMode _mode)
{
	const int INDEX{ static_cast<int>(_mode) };

	// ���Z����
	float blendFactor[]{ D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_ZERO };
	pContext_->OMSetBlendState(pBlendState_[INDEX].Get(), blendFactor, 0xffffffffU);

	// �[�x�X�e���V���ւ̏�������
	pContext_->OMSetDepthStencilState(pDepthStencilState_[INDEX].Get(), 0);
}

void mtgb::DirectX11Draw::SetIsWriteToDepthBuffer(const bool _enabled)
{
	if (_enabled)
	{
		// �[�x�o�b�t�@���w�肷��
		pContext_->OMSetRenderTargets(1, pRenderTargetView_.GetAddressOf(), pDepthStencilView_.Get());

	}
	else
	{
		// �[�x�o�b�t�@���O�� nullptr���w�肷��
		pContext_->OMSetRenderTargets(1, pRenderTargetView_.GetAddressOf(), nullptr);
	}
}

void mtgb::DirectX11Draw::Begin()
{
	// �`��r���[�N���A
	pContext_->ClearRenderTargetView(pRenderTargetView_.Get(), backgroundColor_.f);

	// �[�x�o�b�t�@�N���A
	pContext_->ClearDepthStencilView(pDepthStencilView_.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0U);
}

void mtgb::DirectX11Draw::End()
{
	// �X���b�v���ĉ�ʍX�V
	//pSwapChain_->Present(0U, 0U);
	HRESULT hr = pSwapChain1_->Present(0U, 0U);
	if (FAILED(hr))
	{
		LOGIMGUI_CAT("Device", "failed SwapChain::Present:Error-%ld");
	}
	//pSwapChain_->Present(0U, 0U);
}

void mtgb::DirectX11Draw::Release()
{
	for (auto& depthStencilState : pDepthStencilState_)
	{
		depthStencilState.Reset();
	}
	for (auto& blendState : pBlendState_)
	{
		blendState.Reset();
	}
	pDepthStencilView_.Reset();
	pRenderTargetView_.Reset();
	pDXGIDevice_.Reset();
	for (auto& adapter : pDXGIAdapters_)
	{
		adapter.Reset();
	}
	pDXGIFactory_.Reset();
	SAFE_RELEASE(pSwapChain_);
	pSwapChain1_.Reset();
	pContext_.Reset();

	ID3D11Debug* pDebug = nullptr;

	if (SUCCEEDED(pDevice_->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&pDebug))))
	{
		// D3D11_RLO_DETAIL�ŏڍׂȃ��|�[�g
		pDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		pDebug->Release();
	}
	pDevice_.Reset();
}

mtgb::ShaderBundle::~ShaderBundle()
{
	pVertexLayout.Reset();
	pVertexShader.Reset();
	pPixelShader.Reset();
	pRasterizerState.Reset();
}
