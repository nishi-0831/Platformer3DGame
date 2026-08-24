#include "DirectX11Manager.h"
#include "Core/Game.h"
#include "Utility/MTAssert.h"
#include "IncludingWindows.h"
#include <d3d11.h>

#include <dxgi.h>
#include <DirectXMath.h>
#include "Screen.h"
#include "WindowContext/WindowContext.h"
#include "Utility/ReleaseUtility.h"
#include "Direct2D/Direct2D.h"
#include "Editor/MTImGui.h"
#include "Graphics/DirectX11Draw.h"

mtgb::DirectX11Manager::DirectX11Manager() {}

mtgb::DirectX11Manager::~DirectX11Manager() {}

void mtgb::DirectX11Manager::Initialize()
{

	InitializeCommonResources();
}

void mtgb::DirectX11Manager::Update()
{
	MTImGui::DirectShow(
		[this]()
		{
			for (auto& desc : adaptersDesc_)
			{
				ImGui::PushID(&desc);
				PropertyDisplayRegistry::Instance().ShowProperty(&desc, "AdapterDesc");
				ImGui::PopID();
				ImGui::Separator();
			}

			// モニター(DXGIOutput)の情報
			for (auto& monitorInfo : DirectX11Draw::monitorInfos_)
			{
				ImGui::PushID(&monitorInfo);
				ImGui::LabelText("adapterIndex", "%d", monitorInfo.adapterIndex);
				ImGui::LabelText("outputIndex", "%d", monitorInfo.outputIndex);
				PropertyDisplayRegistry::Instance().ShowProperty(&monitorInfo.desc, "OutputDesc");
				ImGui::PopID();
				ImGui::Separator();
			}
		},
		"Adapter,OutputDesc",
		ShowType::SETTINGS
	);
}

void mtgb::DirectX11Manager::InitializeCommonResources()
{
	HRESULT hResult {};

	D3D_FEATURE_LEVEL level {};

	massert(SUCCEEDED(hResult) && "QueryInterfaceに失敗 @DirectX11Manager::InitializeCommonResources");

	hResult = CreateDXGIFactory1(
		_uuidof(IDXGIFactory2),
		reinterpret_cast<void**>(DirectX11Draw::pDXGIFactory_.ReleaseAndGetAddressOf())
	);
	massert(SUCCEEDED(hResult) && "CreateDXGIFactory1に失敗 @DirectX11Manager::InitializeCommonResources");

	UINT i						   = 0;
	ComPtr<IDXGIAdapter1> pAdapter = nullptr;
	while (DirectX11Draw::pDXGIFactory_->EnumAdapters1(i, pAdapter.GetAddressOf()) != DXGI_ERROR_NOT_FOUND)
	{
		DirectX11Draw::pDXGIAdapters_.push_back(pAdapter);
		++i;

		DXGI_ADAPTER_DESC1 desc;
		pAdapter->GetDesc1(&desc);
		adaptersDesc_.push_back(desc);
	}

	EnumAvailableMonitors(); // モニターの列挙

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
	massert(SUCCEEDED(hResult) && "D3D11CreateDeviceに失敗 @DirectX11Manager::InitializeCommonResources");

	hResult = DirectX11Draw::pDevice_->QueryInterface(
		_uuidof(IDXGIDevice1),
		(void**)DirectX11Draw::pDXGIDevice_.ReleaseAndGetAddressOf()
	);
	massert(SUCCEEDED(hResult) && "QueryInterfaceに失敗 @DirectX11Manager::InitializeCommonResources");
	// InitializeShaderBundle(); // シェーダバンドルの初期化

	const D3D11_SAMPLER_DESC SAMPLER_DESC {
		.Filter	  = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		.AddressU = D3D11_TEXTURE_ADDRESS_WRAP,
		.AddressV = D3D11_TEXTURE_ADDRESS_WRAP,
		.AddressW = D3D11_TEXTURE_ADDRESS_WRAP,
	};

	hResult = DirectX11Draw::pDevice_->CreateSamplerState(
		&SAMPLER_DESC,
		DirectX11Draw::pDefaultSamplerState_.ReleaseAndGetAddressOf()
	);
	massert(SUCCEEDED(hResult) && "デフォルトのサンプラ作成に失敗 @DirectX11Manager::InitializeCommonResources");
#pragma region 深度ステンシルステート作成

	// BlendMode::Defaultの作成
	D3D11_DEPTH_STENCIL_DESC DEPTH_STENCIL_DESC {
		.DepthEnable	  = TRUE, // 深度テストを行うかどうか
		.DepthWriteMask	  = D3D11_DEPTH_WRITE_MASK_ALL,
		.DepthFunc		  = D3D11_COMPARISON_LESS_EQUAL, // 深度の比較方法 : LESS_EQUALは深度が元データ以下の場合に成功
		.StencilEnable	  = TRUE,						 // ステンシルテストを行うかどうか
		.StencilReadMask  = {},
		.StencilWriteMask = {},
		.FrontFace {
			// カメラを向いているピクセルの深度、ステンシルテストの結果に対する操作を指定
			.StencilFailOp		= D3D11_STENCIL_OP_KEEP, // ステンシルテスト失敗時
			.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP, // ステンシルテスト成功、深度テスト失敗時
			.StencilPassOp		= D3D11_STENCIL_OP_KEEP, // 深度、ステンシルの両方のテストに成功時
			.StencilFunc =
				D3D11_COMPARISON_ALWAYS, // ステンシルデータと既存のステンシルデータを比較する関数(公式のをコピペ)
		},
		.BackFace // カメラを向いていないピクセルの深度、ステンシルテストの結果に対する操作を指定
		{
			.StencilFailOp		= D3D11_STENCIL_OP_KEEP,
			.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP,
			.StencilPassOp		= D3D11_STENCIL_OP_KEEP,
			.StencilFunc		= D3D11_COMPARISON_ALWAYS,
		}
	};

	hResult = DirectX11Draw::pDevice_->CreateDepthStencilState(
		&DEPTH_STENCIL_DESC,
		DirectX11Draw::pDepthStencilState_[static_cast<size_t>(StencilMode::DEFAULT)].ReleaseAndGetAddressOf()
	);

	massert(
		SUCCEEDED(hResult) // 深度ステンシルステートの作成に成功
		&& "BlendMode::Defaultの深度ステンシルステートの作成に失敗 @DirectX11Manager::InitializeCommonResources"
	);

	// StencilMode::WriteSelected
	DEPTH_STENCIL_DESC = { .DepthEnable		 = TRUE, // 深度テストを行うかどうか
						   .DepthWriteMask	 = D3D11_DEPTH_WRITE_MASK_ALL,
						   .DepthFunc		 = D3D11_COMPARISON_LESS_EQUAL,
						   .StencilEnable	 = TRUE, // ステンシルテストを行うかどうか
						   .StencilReadMask	 = 0xFF,
						   .StencilWriteMask = 0xFF,
						   .FrontFace {
							   .StencilFailOp	   = D3D11_STENCIL_OP_KEEP,
							   .StencilDepthFailOp = D3D11_STENCIL_OP_KEEP,
							   .StencilPassOp	   = D3D11_STENCIL_OP_REPLACE,
							   .StencilFunc		   = D3D11_COMPARISON_ALWAYS,
						   },
						   .BackFace {
							   .StencilFailOp	   = D3D11_STENCIL_OP_KEEP,
							   .StencilDepthFailOp = D3D11_STENCIL_OP_KEEP,
							   .StencilPassOp	   = D3D11_STENCIL_OP_REPLACE,
							   .StencilFunc		   = D3D11_COMPARISON_ALWAYS,
						   } };

	hResult = DirectX11Draw::pDevice_->CreateDepthStencilState(
		&DEPTH_STENCIL_DESC,
		DirectX11Draw::pDepthStencilState_[static_cast<size_t>(StencilMode::WriteSelected)].ReleaseAndGetAddressOf()
	);

	massert(
		SUCCEEDED(hResult) // 深度ステンシルステートの作成に成功
		&& "BlendMode::Spriteの深度ステンシルステートの作成に失敗 @DirectX11Manager::InitializeCommonResources"
	);

	// StencilMode::DrawOutline
	DEPTH_STENCIL_DESC = { .DepthEnable		 = TRUE, // 深度テストを行うかどうか
						   .DepthWriteMask	 = D3D11_DEPTH_WRITE_MASK_ALL,
						   .DepthFunc		 = D3D11_COMPARISON_LESS_EQUAL,
						   .StencilEnable	 = TRUE, // ステンシルテストを行うかどうか
						   .StencilReadMask	 = 0xFF,
						   .StencilWriteMask = 0xFF,
						   .FrontFace {
							   .StencilFailOp	   = D3D11_STENCIL_OP_KEEP,
							   .StencilDepthFailOp = D3D11_STENCIL_OP_KEEP,
							   .StencilPassOp	   = D3D11_STENCIL_OP_KEEP,
							   .StencilFunc		   = D3D11_COMPARISON_NOT_EQUAL,
						   },
						   .BackFace {
							   .StencilFailOp	   = D3D11_STENCIL_OP_KEEP,
							   .StencilDepthFailOp = D3D11_STENCIL_OP_KEEP,
							   .StencilPassOp	   = D3D11_STENCIL_OP_KEEP,
							   .StencilFunc		   = D3D11_COMPARISON_NOT_EQUAL,
						   } };

	hResult = DirectX11Draw::pDevice_->CreateDepthStencilState(
		&DEPTH_STENCIL_DESC,
		DirectX11Draw::pDepthStencilState_[static_cast<size_t>(StencilMode::DrawOutline)].ReleaseAndGetAddressOf()
	);

	massert(
		SUCCEEDED(hResult) // 深度ステンシルステートの作成に成功
		&& "BlendMode::Spriteの深度ステンシルステートの作成に失敗 @DirectX11Manager::InitializeCommonResources"
	);
#pragma endregion
#pragma region ブレンドステート作成

	// BlendMode::Defaultの作成
	const D3D11_BLEND_DESC BLEND_DESC {
		.AlphaToCoverageEnable	= FALSE,
		.IndependentBlendEnable = FALSE,
		.RenderTarget {
			D3D11_RENDER_TARGET_BLEND_DESC {
				.BlendEnable		   = TRUE,
				.SrcBlend			   = D3D11_BLEND_SRC_ALPHA,
				.DestBlend			   = D3D11_BLEND_INV_SRC_ALPHA,
				.BlendOp			   = D3D11_BLEND_OP_ADD,
				.SrcBlendAlpha		   = D3D11_BLEND_ONE,
				.DestBlendAlpha		   = D3D11_BLEND_ZERO,
				.BlendOpAlpha		   = D3D11_BLEND_OP_ADD,
				.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL,
			},
		},
	};

	hResult = DirectX11Draw::pDevice_->CreateBlendState(
		&BLEND_DESC,
		DirectX11Draw::pBlendState_[static_cast<size_t>(BlendMode::DEFAULT)].ReleaseAndGetAddressOf()
	);

	massert(
		SUCCEEDED(hResult) // ブレンドステート作成に成功
		&& "BlendMode::Defaultのブレンドステート作成に失敗 @DirectX11Manager::InitializeCommonResources"
	);

	// BlendMode::Spriteの作成
	// 設定はBlendMode::Defaultと同じ
	hResult = DirectX11Draw::pDevice_->CreateBlendState(
		&BLEND_DESC,
		DirectX11Draw::pBlendState_[static_cast<size_t>(BlendMode::SPRITE)].ReleaseAndGetAddressOf()
	);

	massert(
		SUCCEEDED(hResult) // ブレンドステート作成に成功
		&& "ブレンドステート作成に失敗 @DirectX11Manager::InitializeCommonResources"
	);

#pragma endregion
	DirectX11Draw::pContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void mtgb::DirectX11Manager::CreateDXGISurface(IDXGISwapChain1* _pSwapChain1, IDXGISurface** _ppDxgiSurface)
{
	HRESULT hResult {};

	// バックバッファ受け取る
	ComPtr<ID3D11Texture2D> pBackBuffer = nullptr;
	hResult =
		_pSwapChain1->GetBuffer(0, _uuidof(ID3D11Texture2D), reinterpret_cast<void**>(pBackBuffer.GetAddressOf()));
	massert(SUCCEEDED(hResult) && "GetBufferに失敗 @DirectX11Manager::CreateDXGISurface");

	// バックバッファからIDXGISurfaceインターフェースを取り出す
	hResult = pBackBuffer->QueryInterface(IID_PPV_ARGS(_ppDxgiSurface));

	pBackBuffer.Reset();

	massert(SUCCEEDED(hResult) && "QueryInterfaceに失敗 @DirectX11Manager::CreateDXGISurface");
}

void mtgb::DirectX11Manager::CreateSwapChain(HWND _hWnd, IDXGIOutput* _pOutput, IDXGISwapChain1** _ppSwapChain1)
{
	HRESULT hResult {};

	const Vector2Int SCREEN_SIZE { Game::System<Screen>().GetSize() };

	DXGI_SWAP_CHAIN_DESC1 desc {
		.Width	= static_cast<UINT>(SCREEN_SIZE.x), // 解像度(ピクセル数)。0ならウィンドウのサイズに合わせる
		.Height = static_cast<UINT>(SCREEN_SIZE.y), // 解像度(ピクセル数)。0ならウィンドウのサイズに合わせる
		.Format = DXGI_FORMAT_R8G8B8A8_UNORM,		// 使える色数
		.Stereo = FALSE,							// ステレオ(3D立体視)表示を有効にするか
		.SampleDesc {
			.Count	 = 1,
			.Quality = 0,
		},
		.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
		.BufferCount = 2, // 裏画面の枚数
		.Scaling	 = DXGI_SCALING_STRETCH,
		.SwapEffect	 = DXGI_SWAP_EFFECT_FLIP_DISCARD,
		.AlphaMode	 = DXGI_ALPHA_MODE_UNSPECIFIED,
		.Flags		 = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
	};

	// 仮のフラグ、
	bool fullscreen								   = false;
	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc = {
		.RefreshRate { .Numerator = 60, .Denominator = 1 },
		.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
		.Scaling		  = DXGI_MODE_SCALING_UNSPECIFIED,
		.Windowed		  = FALSE, // フルスクリーン
	};

	if (fullscreen)
	{
		hResult = DirectX11Draw::pDXGIFactory_->CreateSwapChainForHwnd(
			DirectX11Draw::pDevice_.Get(),
			_hWnd,
			&desc,
			&fullscreenDesc, // フルスクリーンの設定
			_pOutput,		 // 出力
			_ppSwapChain1
		);
	}
	else
	{
		hResult = DirectX11Draw::pDXGIFactory_->CreateSwapChainForHwnd(
			DirectX11Draw::pDevice_.Get(),
			_hWnd,
			&desc,
			nullptr, // 初期状態をフルスクリーンにしたい場合のみDESCを渡して、そうでないならnullptrにしておいて必要に応じてSetFullscreenStateで切り替える
			_pOutput, // 出力
			_ppSwapChain1
		);
	}
	massert(SUCCEEDED(hResult) && "CreateSwapChainForHwndに失敗 @DirectX11Manager::CreateSwapChain");
}

void mtgb::DirectX11Manager::CreateRenderTargetView(
	IDXGISwapChain1* _pSwapChain1,
	ID3D11RenderTargetView** _ppRenderTargetView
)
{
	HRESULT hResult {};

	ComPtr<ID3D11Texture2D> pBackBuffer { nullptr };
	hResult =
		_pSwapChain1->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(pBackBuffer.GetAddressOf()));
	massert(SUCCEEDED(hResult) && "GetBufferに失敗 @DirectX11Manager::CreateRenderTargetView");

	hResult = DirectX11Draw::pDevice_->CreateRenderTargetView(pBackBuffer.Get(), nullptr, _ppRenderTargetView);
	massert(SUCCEEDED(hResult) && "CreateRenderTargetViewに失敗 @DirectX11Manager::CreateRenderTargetView");

	pBackBuffer.Reset();
}

void mtgb::DirectX11Manager::CreateViewport(const Vector2Int& _size, D3D11_VIEWPORT& _viewport)
{
	_viewport = {
		.TopLeftX = 0,
		.TopLeftY = 0,
		.Width	  = static_cast<float>(_size.x),
		.Height	  = static_cast<float>(_size.y),
		.MinDepth = 0,
		.MaxDepth = 1,
	};
}

void mtgb::DirectX11Manager::CreateDepthStencilAndDepthStencilView(
	const Vector2Int& _bufSize,
	ID3D11Texture2D** _ppDepthStencil,
	ID3D11DepthStencilView** _ppDepthStencilView
)
{
	HRESULT hResult {};

	// const Vector2Int SCREEN_SIZE{ Game::System<Screen>().GetSize() };

	// 深度バッファの設定
	const D3D11_TEXTURE2D_DESC DEPTH_TEXTURE2D_DESC {
		.Width	   = static_cast<UINT>(_bufSize.x),
		.Height	   = static_cast<UINT>(_bufSize.y),
		.MipLevels = 1,
		.ArraySize = 1,
		.Format	   = DXGI_FORMAT_D24_UNORM_S8_UINT,
		.SampleDesc { .Count = 1, .Quality = 0 },
		.Usage			= D3D11_USAGE_DEFAULT,
		.BindFlags		= D3D11_BIND_DEPTH_STENCIL,
		.CPUAccessFlags = 0,
		.MiscFlags		= 0,
	};

	hResult = DirectX11Draw::pDevice_->CreateTexture2D(&DEPTH_TEXTURE2D_DESC, nullptr, _ppDepthStencil);

	massert(
		SUCCEEDED(hResult) // 深度ステンシルバッファの作成に失敗
		&& "深度ステンシルバッファの作成に失敗"
	);

	hResult = DirectX11Draw::pDevice_->CreateDepthStencilView(*_ppDepthStencil, nullptr, _ppDepthStencilView);

	massert(
		SUCCEEDED(hResult) // 深度ステンシルビュの作成に成功
		&& "深度ステンシルビューの作成に失敗"
	);
}

void mtgb::DirectX11Manager::ChangeViewport(const D3D11_VIEWPORT& _viewport)
{
	DirectX11Draw::pContext_->RSSetViewports(1, &_viewport);
}

void mtgb::DirectX11Manager::ChangeRenderTargets(
	ComPtr<ID3D11RenderTargetView> _pRenderTargetView,
	ComPtr<ID3D11DepthStencilView> _pDepthStencilView
)
{
	DirectX11Draw::pRenderTargetView_ = _pRenderTargetView;
	DirectX11Draw::pDepthStencilView_ = _pDepthStencilView;

	DirectX11Draw::pContext_->OMSetRenderTargets(1, _pRenderTargetView.GetAddressOf(), _pDepthStencilView.Get());
}

void mtgb::DirectX11Manager::ChangeSwapChain(ComPtr<IDXGISwapChain1> _pSwapChain1)
{
	DirectX11Draw::pSwapChain1_ = _pSwapChain1;
}

std::optional<mtgb::MonitorInfo> mtgb::DirectX11Manager::AssignAvailableMonitor(IDXGIOutput** _ppOutput)
{
	// 初回の列挙
	/*if (DirectX11Draw::monitorInfos_.empty())
	{
		EnumAvailableMonitors();
	}*/

	// 未使用のモニターを探す
	for (auto& info : DirectX11Draw::monitorInfos_)
	{
		// すでに使用済み、もしくはアダプター、モニターのインデックスが未設定の場合はスキップ
		if (info.isRequested || info.adapterIndex == MonitorInfo::INVALID_INDEX ||
			info.outputIndex == MonitorInfo::INVALID_INDEX)
			continue;

		// モニターを列挙して、IDXGIOutputの作成を試みる
		HRESULT hResult = DirectX11Draw::pDXGIAdapters_[info.adapterIndex]->EnumOutputs(info.outputIndex, _ppOutput);
		if (SUCCEEDED(hResult))
		{
			// 作成成功
			info.isRequested = true;
			return info;
		}
	}

	// 全て使用済みの場合は、0番目のモニターを返す
	if (!DirectX11Draw::monitorInfos_.empty())
	{
		HRESULT hResult = DirectX11Draw::pDXGIAdapters_[0]->EnumOutputs(0, _ppOutput);
		if (SUCCEEDED(hResult))
		{
			return DirectX11Draw::monitorInfos_[0];
		}
	}

	// モニターの割り当て失敗
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
	// パイプラインにバインドされた全てをリセット
	DirectX11Draw::pContext_->ClearState();
	// 描画コマンドを強制的にGPUに送り出す
	DirectX11Draw::pContext_->Flush();

	Game::System<Direct2D>().Reset();
	DirectX11Draw::pDepthStencil_.Reset();
	DirectX11Draw::pDepthStencilView_.Reset();
	DirectX11Draw::pRenderTargetView_.Reset();
}

void mtgb::DirectX11Manager::SetDefaultStates()
{
	DirectX11Draw::pContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void mtgb::DirectX11Manager::EnumAvailableMonitors()
{
	DirectX11Draw::monitorInfos_.clear();

	ComPtr<IDXGIAdapter1> pAdapter;
	for (UINT adapterIndex = 0; adapterIndex < DirectX11Draw::pDXGIAdapters_.size(); adapterIndex++)
	{
		ComPtr<IDXGIOutput> pOutput;
		UINT outputIndex = 0;
		while (DirectX11Draw::pDXGIAdapters_[adapterIndex]->EnumOutputs(outputIndex, pOutput.GetAddressOf()) !=
			   DXGI_ERROR_NOT_FOUND)
		{

			MonitorInfo info {};
			info.adapterIndex = static_cast<int>(adapterIndex);
			info.outputIndex  = outputIndex;
			info.isRequested  = false;
			HRESULT hResult	  = pOutput->GetDesc(&info.desc);
			if (SUCCEEDED(hResult))
			{
				DirectX11Draw::monitorInfos_.push_back(info);
			}
			pOutput.Reset();
			outputIndex++;
		}
	}
}