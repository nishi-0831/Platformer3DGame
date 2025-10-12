#include "DirectX11Manager.h"
#include "Game.h"
#include "MTAssert.h"
#include "IncludingWindows.h"
#include <d3d11.h>

#include <dxgi.h>
#include <DirectXMath.h>
#include "ImGuiRenderer.h"
#include "MainWindow.h"
#include "Screen.h"
#include "Vector3.h"
#include <d3dcompiler.h>
#include "HLSLInclude.h"
#include "WindowContext.h"
#include "ReleaseUtility.h"
#include "Direct2D/Direct2D.h"
#include "MTImGui.h"
#include "DirectX11Draw.h"

mtgb::DirectX11Manager::DirectX11Manager()
{
}

mtgb::DirectX11Manager::~DirectX11Manager()
{
}

void mtgb::DirectX11Manager::Initialize()
{

	InitializeCommonResources();
}

void mtgb::DirectX11Manager::Update()
{
	//MTImGui::Instance().DirectShow([this]() {
	//		for (auto& desc : adaptersDesc_)
	//		{
	//			ImGui::PushID(&desc);
	//			TypeRegistry::Instance().CallFunc(&desc, "AdapterDesc");
	//			ImGui::PopID();
	//			ImGui::Separator();
	//		}

	//		// ���j�^�[(DXGIOutput)�̏��
	//		for (auto& monitorInfo : DirectX11Draw::monitorInfos_)
	//		{
	//			ImGui::PushID(&monitorInfo);
	//			ImGui::LabelText("adapterIndex", "%d", monitorInfo.adapterIndex);
	//			ImGui::LabelText("outputIndex","%d", monitorInfo.outputIndex);
	//			TypeRegistry::Instance().CallFunc(&monitorInfo.desc, "OutputDesc");
	//			ImGui::PopID();
	//			ImGui::Separator();
	//		}
	//	}, "Adapter,OutputDesc", ShowType::Settings);
}

void mtgb::DirectX11Manager::InitializeCommonResources()
{
	HRESULT hResult{};

	STARTUPINFO startupInfo{};
	GetStartupInfo(&startupInfo);
	int nCmdShow = startupInfo.wShowWindow;
	
	D3D_FEATURE_LEVEL level{};

	massert(SUCCEEDED(hResult)
		&& "QueryInterface�Ɏ��s @DirectX11Manager::InitializeCommonResources");

	hResult = CreateDXGIFactory1(_uuidof(IDXGIFactory2), reinterpret_cast<void**>(DirectX11Draw::pDXGIFactory_.ReleaseAndGetAddressOf()));
	massert(SUCCEEDED(hResult)
		&& "CreateDXGIFactory1�Ɏ��s @DirectX11Manager::InitializeCommonResources");

	UINT i = 0;
	ComPtr<IDXGIAdapter1> pAdapter = nullptr;
	while (DirectX11Draw::pDXGIFactory_->EnumAdapters1(i, pAdapter.GetAddressOf()) != DXGI_ERROR_NOT_FOUND)
	{
		DirectX11Draw::pDXGIAdapters_.push_back(pAdapter);
		++i;

		DXGI_ADAPTER_DESC1 desc;
		pAdapter->GetDesc1(&desc);
		adaptersDesc_.push_back(desc);
	}
	
	EnumAvailableMonitors(); // ���j�^�[�̗�

	hResult = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		DirectX11Draw::pDevice_.ReleaseAndGetAddressOf(),
		&level,
		DirectX11Draw::pContext_.ReleaseAndGetAddressOf()
	);
	massert(SUCCEEDED(hResult)
	 && "D3D11CreateDevice�Ɏ��s @DirectX11Manager::InitializeCommonResources");

	hResult = DirectX11Draw::pDevice_->QueryInterface(_uuidof(IDXGIDevice1), (void**)DirectX11Draw::pDXGIDevice_.ReleaseAndGetAddressOf());

	InitializeShaderBundle();  // �V�F�[�_�o���h���̏�����

	const D3D11_SAMPLER_DESC SAMPLER_DESC
	{
		.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		.AddressU = D3D11_TEXTURE_ADDRESS_WRAP,
		.AddressV = D3D11_TEXTURE_ADDRESS_WRAP,
		.AddressW = D3D11_TEXTURE_ADDRESS_WRAP,
	};

	hResult = DirectX11Draw::pDevice_->CreateSamplerState(&SAMPLER_DESC, DirectX11Draw::pDefaultSamplerState_.ReleaseAndGetAddressOf());
	massert(SUCCEEDED(hResult)
		&& "�f�t�H���g�̃T���v���쐬�Ɏ��s @DirectX11Manager::InitializeCommonResources");
#pragma region �[�x�X�e���V���X�e�[�g�쐬

	// BlendMode::Default�̍쐬
	D3D11_DEPTH_STENCIL_DESC DEPTH_STENCIL_DESC
	{
		.DepthEnable = TRUE,	//�[�x�e�X�g���s�����ǂ���
		.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL,
		.DepthFunc = D3D11_COMPARISON_LESS_EQUAL, // �[�x�̔�r���@ : LESS_EQUAL�͐[�x�����f�[�^�ȉ��̏ꍇ�ɐ���
		.StencilEnable = TRUE,  //�X�e���V���e�X�g���s�����ǂ���
		.StencilReadMask = {},
		.StencilWriteMask = {},
		.FrontFace // �J�����������Ă���s�N�Z���̐[�x�A�X�e���V���e�X�g�̌��ʂɑ΂��鑀����w��
		{
			.StencilFailOp = D3D11_STENCIL_OP_KEEP, // �X�e���V���e�X�g���s��
			.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP, // �X�e���V���e�X�g�����A�[�x�e�X�g���s��
			.StencilPassOp = D3D11_STENCIL_OP_KEEP, // �[�x�A�X�e���V���̗����̃e�X�g�ɐ�����
			.StencilFunc = D3D11_COMPARISON_ALWAYS, // �X�e���V���f�[�^�Ɗ����̃X�e���V���f�[�^���r����֐�(�����̂��R�s�y)
		},
		.BackFace // �J�����������Ă��Ȃ��s�N�Z���̐[�x�A�X�e���V���e�X�g�̌��ʂɑ΂��鑀����w��
		{
			.StencilFailOp = D3D11_STENCIL_OP_KEEP,
			.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP,
			.StencilPassOp = D3D11_STENCIL_OP_KEEP,
			.StencilFunc = D3D11_COMPARISON_ALWAYS,
		}
	};

	hResult = DirectX11Draw::pDevice_->CreateDepthStencilState(
		&DEPTH_STENCIL_DESC,
		DirectX11Draw::pDepthStencilState_[static_cast<size_t>(BlendMode::Default)].ReleaseAndGetAddressOf());

	massert(SUCCEEDED(hResult)  // �[�x�X�e���V���X�e�[�g�̍쐬�ɐ���
		&& "BlendMode::Default�̐[�x�X�e���V���X�e�[�g�̍쐬�Ɏ��s @DirectX11Manager::InitializeCommonResources");

	// BlendMode::Sprite�̍쐬
	DEPTH_STENCIL_DESC =
	{
		.DepthEnable = FALSE,	//�[�x�e�X�g���s�����ǂ���
		.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO,
		.DepthFunc = D3D11_COMPARISON_LESS_EQUAL,
		.StencilEnable = FALSE,  //�X�e���V���e�X�g���s�����ǂ���
		.StencilReadMask = {},
		.StencilWriteMask = {},
		.FrontFace
		{
			.StencilFailOp = D3D11_STENCIL_OP_KEEP,
			.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP,
			.StencilPassOp = D3D11_STENCIL_OP_KEEP,
			.StencilFunc = D3D11_COMPARISON_ALWAYS,
		},
		.BackFace
		{
			.StencilFailOp = D3D11_STENCIL_OP_KEEP,
			.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP,
			.StencilPassOp = D3D11_STENCIL_OP_KEEP,
			.StencilFunc = D3D11_COMPARISON_ALWAYS,
		}
	};

	hResult = DirectX11Draw::pDevice_->CreateDepthStencilState(
		&DEPTH_STENCIL_DESC,
		DirectX11Draw::pDepthStencilState_[static_cast<size_t>(BlendMode::Sprite)].ReleaseAndGetAddressOf());

	massert(SUCCEEDED(hResult)  // �[�x�X�e���V���X�e�[�g�̍쐬�ɐ���
		&& "BlendMode::Sprite�̐[�x�X�e���V���X�e�[�g�̍쐬�Ɏ��s @DirectX11Manager::InitializeCommonResources");
#pragma endregion
#pragma region �u�����h�X�e�[�g�쐬

	// BlendMode::Default�̍쐬
	const D3D11_BLEND_DESC BLEND_DESC
	{
		.AlphaToCoverageEnable = FALSE,
		.IndependentBlendEnable = FALSE,
		.RenderTarget
		{
			D3D11_RENDER_TARGET_BLEND_DESC
			{
				.BlendEnable = TRUE,
				.SrcBlend = D3D11_BLEND_SRC_ALPHA,
				.DestBlend = D3D11_BLEND_INV_SRC_ALPHA,
				.BlendOp = D3D11_BLEND_OP_ADD,
				.SrcBlendAlpha = D3D11_BLEND_ONE,
				.DestBlendAlpha = D3D11_BLEND_ZERO,
				.BlendOpAlpha = D3D11_BLEND_OP_ADD,
				.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL,
			},
		},
	};

	hResult = DirectX11Draw::pDevice_->CreateBlendState(
		&BLEND_DESC,
		DirectX11Draw::pBlendState_[static_cast<size_t>(BlendMode::Default)].ReleaseAndGetAddressOf());

	massert(SUCCEEDED(hResult)  // �u�����h�X�e�[�g�쐬�ɐ���
		&& "BlendMode::Default�̃u�����h�X�e�[�g�쐬�Ɏ��s @DirectX11Manager::InitializeCommonResources");

	// BlendMode::Sprite�̍쐬
	// �ݒ��BlendMode::Default�Ɠ���
	hResult = DirectX11Draw::pDevice_->CreateBlendState(
		&BLEND_DESC,
		DirectX11Draw::pBlendState_[static_cast<size_t>(BlendMode::Sprite)].ReleaseAndGetAddressOf());

	massert(SUCCEEDED(hResult)  // �u�����h�X�e�[�g�쐬�ɐ���
		&& "�u�����h�X�e�[�g�쐬�Ɏ��s @DirectX11Manager::InitializeCommonResources");
	
#pragma endregion
	DirectX11Draw::pContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void mtgb::DirectX11Manager::CreateDXGISurface(IDXGISwapChain1* pSwapChain1, IDXGISurface** ppDXGISurface)
{
	HRESULT hResult{};
	
	//�o�b�N�o�b�t�@�󂯎��
	ComPtr<ID3D11Texture2D> pBackBuffer = nullptr;
	hResult = pSwapChain1->GetBuffer(0, _uuidof(ID3D11Texture2D), reinterpret_cast<void**>(pBackBuffer.GetAddressOf()));
	massert(SUCCEEDED(hResult)
		&& "GetBuffer�Ɏ��s @DirectX11Manager::CreateDXGISurface");

	//�o�b�N�o�b�t�@����IDXGISurface�C���^�[�t�F�[�X�����o��
	hResult = pBackBuffer->QueryInterface(IID_PPV_ARGS(ppDXGISurface));

	pBackBuffer.Reset();

	massert(SUCCEEDED(hResult)
		&& "QueryInterface�Ɏ��s @DirectX11Manager::CreateDXGISurface");
}



void mtgb::DirectX11Manager::CreateSwapChain(HWND hWnd, IDXGIOutput* pOutput, IDXGISwapChain1** ppSwapChain1)
{
	HRESULT hResult{};
	
	const Vector2Int SCREEN_SIZE{ Game::System<Screen>().GetSize() };
	
	DXGI_SWAP_CHAIN_DESC1 desc
	{
		.Width = static_cast<UINT>(SCREEN_SIZE.x),//�𑜓x(�s�N�Z����)�B0�Ȃ�E�B���h�E�̃T�C�Y�ɍ��킹��
		.Height = static_cast<UINT>(SCREEN_SIZE.y),//�𑜓x(�s�N�Z����)�B0�Ȃ�E�B���h�E�̃T�C�Y�ɍ��킹��
		.Format = DXGI_FORMAT_R8G8B8A8_UNORM,  // �g����F��
		.Stereo = FALSE,//�X�e���I(3D���̎�)�\����L���ɂ��邩
		.SampleDesc
		{
			.Count = 1,
			.Quality = 0,
		},
		.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
		.BufferCount = 2,  // ����ʂ̖���
		.Scaling = DXGI_SCALING_STRETCH,
		.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
		.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
		.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
	};

	// ���̃t���O�A
	bool fullscreen = false;
	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc = 
	{
		.RefreshRate
			{
				.Numerator = 60,
				.Denominator = 1
			},
		.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
		.Scaling = DXGI_MODE_SCALING_UNSPECIFIED,
		.Windowed = FALSE, // �t���X�N���[��
	};

	if (fullscreen)
	{
		hResult = DirectX11Draw::pDXGIFactory_->CreateSwapChainForHwnd(
			DirectX11Draw::pDevice_.Get(),
			hWnd,
			&desc,
			&fullscreenDesc,//�t���X�N���[���̐ݒ�
			pOutput,//�o��
			ppSwapChain1
		);
	}
	else
	{
		hResult = DirectX11Draw::pDXGIFactory_->CreateSwapChainForHwnd(
			DirectX11Draw::pDevice_.Get(),
			hWnd,
			&desc,
			nullptr,//������Ԃ��t���X�N���[���ɂ������ꍇ�̂�DESC��n���āA�����łȂ��Ȃ�nullptr�ɂ��Ă����ĕK�v�ɉ�����SetFullscreenState�Ő؂�ւ���
			pOutput,//�o��
			ppSwapChain1
		);
	}
	massert(SUCCEEDED(hResult)
		&& "CreateSwapChainForHwnd�Ɏ��s @DirectX11Manager::CreateSwapChain");
}

void mtgb::DirectX11Manager::CreateRenderTargetView(IDXGISwapChain1* pSwapChain1, ID3D11RenderTargetView** ppRenderTargetView)
{
	HRESULT hResult{};
	
	ComPtr<ID3D11Texture2D> pBackBuffer{ nullptr };
	hResult = pSwapChain1->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(pBackBuffer.GetAddressOf()));
	massert(SUCCEEDED(hResult) 
		&& "GetBuffer�Ɏ��s @DirectX11Manager::CreateRenderTargetView");

	hResult = DirectX11Draw::pDevice_->CreateRenderTargetView(pBackBuffer.Get(), nullptr, ppRenderTargetView);
	massert(SUCCEEDED(hResult)
		&& "CreateRenderTargetView�Ɏ��s @DirectX11Manager::CreateRenderTargetView");

	pBackBuffer.Reset();
}

void mtgb::DirectX11Manager::CreateViewport(const Vector2Int& size, D3D11_VIEWPORT& viewport)
{
	viewport =
	{
		.TopLeftX = 0,
		.TopLeftY = 0,
		.Width = static_cast<float>(size.x),
		.Height = static_cast<float>(size.y),
		.MinDepth = 0,
		.MaxDepth = 1,
	};
}

void mtgb::DirectX11Manager::CreateDepthStencilAndDepthStencilView(const Vector2Int bufSize, ID3D11Texture2D** ppDepthStencil, ID3D11DepthStencilView** ppDepthStencilView)
{
	HRESULT hResult{};
	
	//const Vector2Int SCREEN_SIZE{ Game::System<Screen>().GetSize() };
	
	// �[�x�o�b�t�@�̐ݒ�
	const D3D11_TEXTURE2D_DESC DEPTH_TEXTURE2D_DESC
	{
		.Width = static_cast<UINT>(bufSize.x),
		.Height = static_cast<UINT>(bufSize.y),
		.MipLevels = 1,
		.ArraySize = 1,
		.Format = DXGI_FORMAT_D32_FLOAT,
		.SampleDesc
		{
			.Count = 1,
			.Quality = 0
		},
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_DEPTH_STENCIL,
		.CPUAccessFlags = 0,
		.MiscFlags = 0,
	};

	hResult = DirectX11Draw::pDevice_->CreateTexture2D(
		&DEPTH_TEXTURE2D_DESC,
		nullptr,
		ppDepthStencil);

	massert(SUCCEEDED(hResult)  // �[�x�X�e���V���o�b�t�@�̍쐬�Ɏ��s
		&& "�[�x�X�e���V���o�b�t�@�̍쐬�Ɏ��s");

	hResult = DirectX11Draw::pDevice_->CreateDepthStencilView(
		*ppDepthStencil,
		nullptr,
		ppDepthStencilView);

	massert(SUCCEEDED(hResult)  // �[�x�X�e���V���r���̍쐬�ɐ���
		&& "�[�x�X�e���V���r���[�̍쐬�Ɏ��s");
}



void mtgb::DirectX11Manager::ChangeViewport(const D3D11_VIEWPORT& viewport)
{
	DirectX11Draw::pContext_->RSSetViewports(1, &viewport);
}

void mtgb::DirectX11Manager::ChangeRenderTargets(ComPtr<ID3D11RenderTargetView> pRenderTargetView, ComPtr<ID3D11DepthStencilView> pDepthStencilView)
{
	DirectX11Draw::pRenderTargetView_ = pRenderTargetView;
	DirectX11Draw::pDepthStencilView_ = pDepthStencilView;

	DirectX11Draw::pContext_->OMSetRenderTargets(1, pRenderTargetView.GetAddressOf(), pDepthStencilView.Get());
}

void mtgb::DirectX11Manager::ChangeSwapChain(ComPtr<IDXGISwapChain1> pSwapChain1)
{
	DirectX11Draw::pSwapChain1_ = pSwapChain1;
}

std::optional<mtgb::MonitorInfo> mtgb::DirectX11Manager::AssignAvailableMonitor(IDXGIOutput** ppOutput)
{
	// ����̗�
	/*if (DirectX11Draw::monitorInfos_.empty())
	{
		EnumAvailableMonitors();
	}*/

	// ���g�p�̃��j�^�[��T��
	for (auto& info : DirectX11Draw::monitorInfos_)
	{
		if (!info.isRequested)
		{
			HRESULT hResult = DirectX11Draw::pDXGIAdapters_[info.adapterIndex]->EnumOutputs(info.outputIndex, ppOutput);
			if (SUCCEEDED(hResult))
			{
				info.isRequested = true;
				return info;
			}
		}
	}

	// �S�Ďg�p�ς݂̏ꍇ�́A0�Ԗڂ̃��j�^�[��Ԃ�
	if (!DirectX11Draw::monitorInfos_.empty())
	{
		HRESULT hResult = DirectX11Draw::pDXGIAdapters_[0]->EnumOutputs(0, ppOutput);
		if (SUCCEEDED(hResult))
		{
			return DirectX11Draw::monitorInfos_[0];
		}
	}

	// ���j�^�[�̊��蓖�Ď��s
	return std::nullopt;
}

int mtgb::DirectX11Manager::GetAvailableMonitorCount() const
{
	return static_cast<int>(DirectX11Draw::monitorInfos_.size());
}

void mtgb::DirectX11Manager::Release()
{
	DirectX11Draw::Release();
}

void mtgb::DirectX11Manager::ClearState()
{
	// �p�C�v���C���Ƀo�C���h���ꂽ�S�Ă����Z�b�g
	DirectX11Draw::pContext_->ClearState();
	// �`��R�}���h�������I��GPU�ɑ���o��
	DirectX11Draw::pContext_->Flush();

	//ID3D11CommandList* pCmdList = nullptr;
	//// �`��R�}���h���c���Ă���Ȃ�j��
	//DirectX11Draw::pContext_->FinishCommandList(FALSE, &pCmdList);
	//if (pCmdList)
	//{
	//	pCmdList->Release();
	//}
	Game::System<Direct2D>().Reset();
	DirectX11Draw::pDepthStencil_.Reset();
	DirectX11Draw::pDepthStencilView_.Reset();
	DirectX11Draw::pRenderTargetView_.Reset();
	//DirectX11Draw::pSwapChain1_.Reset();
}

void mtgb::DirectX11Manager::SetDefaultStates()
{
	// PrimitiveTopology ���Đݒ�
	DirectX11Draw::pContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	// �K�v�ɉ����đ��̃f�t�H���g��Ԃ��Đݒ�
	// ��F�f�t�H���g�T���v���[�X�e�[�g�A�u�����h�X�e�[�g�Ȃ�
	/*DirectX11Draw::pContext_->PSGetSamplers*/
}

void mtgb::DirectX11Manager::EnumAvailableMonitors()
{
	DirectX11Draw::monitorInfos_.clear();

	ComPtr<IDXGIAdapter1> pAdapter;
	for (UINT adapterIndex = 0; adapterIndex < DirectX11Draw::pDXGIAdapters_.size();adapterIndex++)
	{
		ComPtr<IDXGIOutput> pOutput;
		UINT outputIndex = 0;
		while(DirectX11Draw::pDXGIAdapters_[adapterIndex]->EnumOutputs(outputIndex, pOutput.GetAddressOf()) != DXGI_ERROR_NOT_FOUND)
		{
		
			MonitorInfo info{};
			info.adapterIndex = static_cast<int>(adapterIndex);
			info.outputIndex = outputIndex;
			info.isRequested = false;
			HRESULT hResult = pOutput->GetDesc(&info.desc);
			if (SUCCEEDED(hResult))
			{
				DirectX11Draw::monitorInfos_.push_back(info);
			}
			pOutput.Reset();
			outputIndex++;
		}
	}
}

void mtgb::DirectX11Manager::InitializeShaderBundle()
{
	DWORD vectorSize{ sizeof(Vector3) };

	CD3D11_RASTERIZER_DESC cRasterizerDesc{};

	// 2D���ʂ̃C���v�b�g���C�A�E�g
	const D3D11_INPUT_ELEMENT_DESC INPUT_ELEMENT_DESC_2D[]
	{
		{
			.SemanticName = "POSITION",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32B32_FLOAT,
			.InputSlot = 0,
			.AlignedByteOffset = vectorSize * 0,
			.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0,
		},
		{
			.SemanticName = "TEXCOORD",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32_FLOAT,
			.InputSlot = 0,
			.AlignedByteOffset = vectorSize * 1,
			.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0,
		},
	};

	// 3D���ʂ̃C���v�b�g���C�A�E�g
	const D3D11_INPUT_ELEMENT_DESC INPUT_ELEMENT_DESC_3D[]
	{
		{
			.SemanticName = "POSITION",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32B32_FLOAT,
			.InputSlot = 0,
			.AlignedByteOffset = vectorSize * 0,
			.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0,
		},
		{
			.SemanticName = "NORMAL",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32B32_FLOAT,
			.InputSlot = 0,
			.AlignedByteOffset = vectorSize * 1,
			.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0,
		},
		{
			.SemanticName = "TEXCOORD",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32_FLOAT,
			.InputSlot = 0,
			.AlignedByteOffset = vectorSize * 2,
			.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0,
		},
	};

	const D3D11_INPUT_ELEMENT_DESC INPUT_ELEMENT_DESC_SKINNED[]
	{
		{
			.SemanticName = "POSITION",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32B32_FLOAT,
			.InputSlot = 0,
			.AlignedByteOffset = vectorSize * 0,
			.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0,
		},
		{
			.SemanticName = "NORMAL",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32B32_FLOAT,
			.InputSlot = 0,
			.AlignedByteOffset = vectorSize * 1,
			.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0,
		},
		{
			.SemanticName = "TEXCOORD",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32_FLOAT,
			.InputSlot = 0,
			.AlignedByteOffset = vectorSize * 2,
			.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0,
		},
		{
			.SemanticName = "BONE_INDEX",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32B32A32_UINT,
			.InputSlot = 0,
			.AlignedByteOffset = vectorSize * 3,
			.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0,
		},
		{
			.SemanticName = "BONE_WEIGHT",
			.SemanticIndex = 0,
			.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
			.InputSlot = 0,
			.AlignedByteOffset = vectorSize * 4,
			.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
			.InstanceDataStepRate = 0,
		},
	};

	// 2D�}�`�p�V�F�[�_�̓ǂݍ���
	{
		cRasterizerDesc = CD3D11_RASTERIZER_DESC(D3D11_RASTERIZER_DESC
		{
			.FillMode = D3D11_FILL_SOLID,   // �h��Ԃ�: solid
			.CullMode = D3D11_CULL_BACK,    // �J�����O: �A�ʏ���
			.FrontCounterClockwise = TRUE,  // �O�p�`�̐��ʌ��� = ���v���
			.DepthBias = {},
			.DepthBiasClamp = {},
			.SlopeScaledDepthBias = {},
			.DepthClipEnable = true,        // �N���b�s���O��L���ɂ���
			.ScissorEnable = {},
			.MultisampleEnable = {},
			.AntialiasedLineEnable = {},
		});

		CompileShader(
			L"Shader/Figure.hlsl",
			ShaderType::Figure,
			INPUT_ELEMENT_DESC_2D,
			sizeof(INPUT_ELEMENT_DESC_2D) / sizeof(D3D11_INPUT_ELEMENT_DESC),
			&cRasterizerDesc);
	}

	// 2D�X�v���C�g�V�F�[�_�̓ǂݍ���
	{
		cRasterizerDesc = CD3D11_RASTERIZER_DESC(D3D11_RASTERIZER_DESC
			{
				.FillMode = D3D11_FILL_SOLID,   // �h��Ԃ�: solid
				.CullMode = D3D11_CULL_NONE,    // �J�����O: �A�ʏ���
				.FrontCounterClockwise = TRUE,  // �O�p�`�̐��ʌ��� = ���v���
				.DepthBias = {},
				.DepthBiasClamp = {},
				.SlopeScaledDepthBias = {},
				.DepthClipEnable = true,        // �N���b�s���O��L���ɂ���
				.ScissorEnable = {},
				.MultisampleEnable = {},
				.AntialiasedLineEnable = {},
			});

		CompileShader(
			L"Shader/Sprite.hlsl",
			ShaderType::Sprite2D,
			INPUT_ELEMENT_DESC_2D,
			sizeof(INPUT_ELEMENT_DESC_2D) / sizeof(D3D11_INPUT_ELEMENT_DESC),
			&cRasterizerDesc);
	}

	// FbxParts�V�F�[�_�̓ǂݍ���
	{
		cRasterizerDesc = CD3D11_RASTERIZER_DESC(D3D11_RASTERIZER_DESC
			{
				.FillMode = D3D11_FILL_SOLID,   // �h��Ԃ�: solid
				.CullMode = D3D11_CULL_BACK,    // �J�����O: �A�ʏ���
				.FrontCounterClockwise = TRUE,  // �O�p�`�̐��ʌ��� = ���v���
				.DepthBias = {},
				.DepthBiasClamp = {},
				.SlopeScaledDepthBias = {},
				.DepthClipEnable = true,        // �N���b�s���O��L���ɂ���
				.ScissorEnable = {},
				.MultisampleEnable = {},
				.AntialiasedLineEnable = {},
			});

		CompileShader(
			L"Shader/FbxParts.hlsl",
			ShaderType::FbxParts,
			INPUT_ELEMENT_DESC_3D,
			sizeof(INPUT_ELEMENT_DESC_3D) / sizeof(D3D11_INPUT_ELEMENT_DESC),
			&cRasterizerDesc);
	}

	// Unlit3D�V�F�[�_�̓ǂݍ���
	{
		cRasterizerDesc = CD3D11_RASTERIZER_DESC(D3D11_RASTERIZER_DESC
			{
				.FillMode = D3D11_FILL_SOLID,   // �h��Ԃ�: solid
				.CullMode = D3D11_CULL_BACK,    // �J�����O: �A�ʏ���
				.FrontCounterClockwise = TRUE,  // �O�p�`�̐��ʌ��� = ���v���
				.DepthBias = {},
				.DepthBiasClamp = {},
				.SlopeScaledDepthBias = {},
				.DepthClipEnable = true,        // �N���b�s���O��L���ɂ���
				.ScissorEnable = {},
				.MultisampleEnable = {},
				.AntialiasedLineEnable = {},
			});

		CompileShader(
			L"Shader/Unlit3D.hlsl",
			ShaderType::Unlit3D,
			INPUT_ELEMENT_DESC_3D,
			sizeof(INPUT_ELEMENT_DESC_3D) / sizeof(D3D11_INPUT_ELEMENT_DESC),
			&cRasterizerDesc);
	}

	// Debug3D�V�F�[�_�̓ǂݍ���
	{
		cRasterizerDesc = CD3D11_RASTERIZER_DESC(D3D11_RASTERIZER_DESC
			{
				.FillMode = D3D11_FILL_WIREFRAME,  // �g����: wireframe
				.CullMode = D3D11_CULL_NONE,       // �J�����O: �B�ʏ������Ȃ�
				.FrontCounterClockwise = TRUE,     // �O�p�`�̐��ʌ��� = ���v���
				.DepthBias = {},
				.DepthBiasClamp = {},
				.SlopeScaledDepthBias = {},
				.DepthClipEnable = true,           // �N���b�s���O��L���ɂ���
				.ScissorEnable = {},
				.MultisampleEnable = {},
				.AntialiasedLineEnable = {},
			});

		CompileShader(
			L"Shader/Debug3D.hlsl",
			ShaderType::Debug3D,
			INPUT_ELEMENT_DESC_3D,
			sizeof(INPUT_ELEMENT_DESC_3D) / sizeof(D3D11_INPUT_ELEMENT_DESC),
			&cRasterizerDesc);
	}

	// �n�`�V�F�[�_�̓ǂݍ���
	{
		cRasterizerDesc = CD3D11_RASTERIZER_DESC(D3D11_RASTERIZER_DESC
			{
				.FillMode = D3D11_FILL_SOLID,   // �h��Ԃ�
				.CullMode = D3D11_CULL_BACK,    // �J�����O: �B�ʏ���
				.FrontCounterClockwise = TRUE,  // �O�p�`�̐��ʌ��� = ���v���
				.DepthBias = {},
				.DepthBiasClamp = {},
				.SlopeScaledDepthBias = {},
				.DepthClipEnable = true,        // �N���b�s���O��L���ɂ���
				.ScissorEnable = {},
				.MultisampleEnable = {},
				.AntialiasedLineEnable = {},
			});

		CompileShader(
			L"Shader/Ground.hlsl",
			ShaderType::Ground,
			INPUT_ELEMENT_DESC_3D,
			sizeof(INPUT_ELEMENT_DESC_3D) / sizeof(D3D11_INPUT_ELEMENT_DESC),
			&cRasterizerDesc);
	}

	// Terrain
	{
		cRasterizerDesc = CD3D11_RASTERIZER_DESC(D3D11_RASTERIZER_DESC
			{
				.FillMode = D3D11_FILL_SOLID,   // �h��Ԃ�
				.CullMode = D3D11_CULL_BACK,    // �J�����O: �B�ʏ���
				.FrontCounterClockwise = TRUE,  // �O�p�`�̐��ʌ��� = ���v���
				.DepthBias = {},
				.DepthBiasClamp = {},
				.SlopeScaledDepthBias = {},
				.DepthClipEnable = true,        // �N���b�s���O��L���ɂ���
				.ScissorEnable = {},
				.MultisampleEnable = {},
				.AntialiasedLineEnable = {},
			});

		CompileShader(
			L"Shader/Terrain.hlsl",
			ShaderType::Terrain,
			INPUT_ELEMENT_DESC_3D,
			sizeof(INPUT_ELEMENT_DESC_3D) / sizeof(D3D11_INPUT_ELEMENT_DESC),
			&cRasterizerDesc);
	}

	// �g���C���V�F�[�_�̓ǂݍ���
	{
		const D3D11_INPUT_ELEMENT_DESC INPUT_ELEMENT_DESC_TRAIL[]
		{
			{
				.SemanticName = "POSITION",
				.SemanticIndex = 0,
				.Format = DXGI_FORMAT_R32G32B32_FLOAT,
				.InputSlot = 0,
				.AlignedByteOffset = vectorSize * 0,
				.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
				.InstanceDataStepRate = 0,
			},
			{
				.SemanticName = "COLOR",
				.SemanticIndex = 0,
				.Format = DXGI_FORMAT_R32G32B32A32_FLOAT,
				.InputSlot = 0,
				.AlignedByteOffset = vectorSize * 1,
				.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA,
				.InstanceDataStepRate = 0,
			},
		}; 
		cRasterizerDesc = CD3D11_RASTERIZER_DESC(D3D11_RASTERIZER_DESC
			{
				.FillMode = D3D11_FILL_SOLID,   // �h��Ԃ�
				.CullMode = D3D11_CULL_NONE,    // �J�����O: �B�ʏ���
				.FrontCounterClockwise = TRUE,  // �O�p�`�̐��ʌ��� = ���v���
				.DepthBias = {},
				.DepthBiasClamp = {},
				.SlopeScaledDepthBias = {},
				.DepthClipEnable = true,        // �N���b�s���O��L���ɂ���
				.ScissorEnable = {},
				.MultisampleEnable = {},
				.AntialiasedLineEnable = {},
			});

		CompileShader(
			L"Shader/Trail.hlsl",
			ShaderType::Trail,
			INPUT_ELEMENT_DESC_TRAIL,
			sizeof(INPUT_ELEMENT_DESC_TRAIL) / sizeof(D3D11_INPUT_ELEMENT_DESC),
			&cRasterizerDesc);

		cRasterizerDesc = CD3D11_RASTERIZER_DESC(D3D11_RASTERIZER_DESC
			{
				.FillMode = D3D11_FILL_SOLID,   // �h��Ԃ�: solid
				.CullMode = D3D11_CULL_BACK,    // �J�����O: �A�ʏ���
				.FrontCounterClockwise = TRUE,  // �O�p�`�̐��ʌ��� = ���v���
				.DepthBias = {},
				.DepthBiasClamp = {},
				.SlopeScaledDepthBias = {},
				.DepthClipEnable = true,        // �N���b�s���O��L���ɂ���
				.ScissorEnable = {},
				.MultisampleEnable = {},
				.AntialiasedLineEnable = {},
			});

		CompileShader(
			L"Shader/FbxPartsSkin.hlsl",
			ShaderType::FbxPartsSkin,
			INPUT_ELEMENT_DESC_SKINNED,
			sizeof(INPUT_ELEMENT_DESC_SKINNED) / sizeof(D3D11_INPUT_ELEMENT_DESC),
			&cRasterizerDesc);
		
	}
}

void mtgb::DirectX11Manager::CompileShader(
	const std::wstring& _fileName,
	const ShaderType& _type,
	const D3D11_INPUT_ELEMENT_DESC* _pHLSLLayout,
	const unsigned int _layoutLength,
	const CD3D11_RASTERIZER_DESC* _pRasterizerDesc)
{
	HLSLInclude hlslInclude{};
	HRESULT hResult{};

#pragma region ���_�V�F�[�_
	// ���_�V�F�[�_�̃C���^�t�F�[�X
	ID3DBlob* pCompileVS{ nullptr };

	// ���_�V�F�[�_�̃R���p�C��
	hResult = D3DCompileFromFile(
		_fileName.c_str(),  // �t�@�C���p�X
		nullptr,            // �V�F�[�_�}�N���̔z��
		&hlslInclude,       // �C���N���[�h������
		"VS",               // �G���g���|�C���g�̊֐���
		"vs_5_0",           // �V�F�[�_�̃o�[�W���� (�I�v�V�����ŕt������)
		0,                  // �I�v�V�����t���O1
		0,                  // �I�v�V�����t���O2
		&pCompileVS,        // �R���p�C���ς݃R�[�h�ւ̃A�N�Z�X�C���^�t�F�[�X
		nullptr);           // �G���[���b�Z�[�W��M�p ����

	massert(SUCCEEDED(hResult)  // ���_�V�F�[�_�̃R���p�C���ɐ���
		&& "���_�V�F�[�_�̃R���p�C���Ɏ��s @DirectX11Manager::CompileShader");

	// ���_�V�F�[�_���쐬���A�w��^�C�v�̃o���h���Ɋi�[����
	hResult = DirectX11Draw::pDevice_->CreateVertexShader(
		pCompileVS->GetBufferPointer(),  // �R���p�C�����ꂽ�o�b�t�@�̃|�C���^
		pCompileVS->GetBufferSize(),     // �o�b�t�@�̃T�C�Y
		nullptr,                         // �����P�[�W�N���X: ����
		DirectX11Draw::shaderBundle_[static_cast<int8_t>(_type)].pVertexShader.ReleaseAndGetAddressOf());

	massert(SUCCEEDED(hResult)  // ���_�V�F�[�_�̍쐬�ɐ���
		&& "���_�V�F�[�_�̍쐬�Ɏ��s @DirectX11Manager::CompileShader");
#pragma endregion

#pragma region �s�N�Z���V�F�[�_
	// �s�N�Z���V�F�[�_�̃C���^�t�F�[�X
	ID3DBlob* pCompilePS{ nullptr };

	// �s�N�Z���V�F�[�_�̃R���p�C��
	hResult = D3DCompileFromFile(
		_fileName.c_str(),  // �t�@�C���p�X
		nullptr,            // �V�F�[�_�}�N���̔z��
		&hlslInclude,       // �C���N���[�h������
		"PS",               // �G���g���|�C���g�̊֐���
		"ps_5_0",           // �V�F�[�_�̃o�[�W���� (�I�v�V�����ŕt������)
		0,                  // �I�v�V�����t���O1
		0,                  // �I�v�V�����t���O2
		&pCompilePS,        // �R���p�C���ς݃R�[�h�ւ̃A�N�Z�X�C���^�t�F�[�X
		nullptr);           // �G���[���b�Z�[�W��M�p ����

	massert(SUCCEEDED(hResult)  // �s�N�Z���V�F�[�_�̃R���p�C���ɐ���
		&& "�s�N�Z���V�F�[�_�̃R���p�C���Ɏ��s @DirectX11Manager::CompileShader");

	// �s�N�Z���V�F�[�_���쐬���A�w��^�C�v�̃o���h���Ɋi�[����
	hResult = DirectX11Draw::pDevice_->CreatePixelShader(
		pCompilePS->GetBufferPointer(),  // �R���p�C�����ꂽ�o�b�t�@�̃|�C���^
		pCompilePS->GetBufferSize(),     // �o�b�t�@�̃T�C�Y
		nullptr,                         // �����P�[�W�N���X: ����
		DirectX11Draw::shaderBundle_[static_cast<int8_t>(_type)].pPixelShader.ReleaseAndGetAddressOf());

	massert(SUCCEEDED(hResult)  // �s�N�Z���V�F�[�_�̍쐬�ɐ���
		&& "�s�N�Z���V�F�[�_�̍쐬�Ɏ��s @DirectX11Manager::CompileShader");
#pragma endregion

#pragma region ���_���C�A�E�g
	// ���_���C�A�E�g���쐬���A�w��^�C�v�̃o���h���Ɋi�[����
	hResult = DirectX11Draw::pDevice_->CreateInputLayout(
		_pHLSLLayout,                    // ���̓f�[�^�^�z��
		_layoutLength,                   // ���̓f�[�^�^�z��̗v�f��
		pCompileVS->GetBufferPointer(),  // �R���p�C�����ꂽ�o�b�t�@�̃|�C���^
		pCompileVS->GetBufferSize(),     // �o�b�t�@�̃T�C�Y
		DirectX11Draw::shaderBundle_[static_cast<int8_t>(_type)].pVertexLayout.ReleaseAndGetAddressOf());

	massert(SUCCEEDED(hResult)  // ���_���C�A�E�g�̍쐬�ɐ���
		&& "���_���C�A�E�g�̍쐬�Ɏ��s @DirectX11Manager::CompileShader");
#pragma endregion

#pragma region ���X�^���C�U
	// ���X�^���C�U���쐬���A�w��^�C�v�̃o���h���Ɋi�[����
	DirectX11Draw::pDevice_->CreateRasterizerState(
		_pRasterizerDesc,  // ���X�^���C�U�̐ݒ�
		DirectX11Draw::shaderBundle_[static_cast<int8_t>(_type)].pRasterizerState.ReleaseAndGetAddressOf());

	massert(SUCCEEDED(hResult)  // ���X�^���C�U�̍쐬�ɐ���
		&& "���X�^���C�U�̍쐬�Ɏ��s @DirectX11Manager::CompileShader");
#pragma endregion


	// ������Ă���
	SAFE_RELEASE(pCompileVS);
	SAFE_RELEASE(pCompilePS);
}
