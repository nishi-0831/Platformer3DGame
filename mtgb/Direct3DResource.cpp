#include "Direct3DResource.h"
#include "ReleaseUtility.h"
#include "Screen.h"
#include "WindowManager.h"
using namespace mtgb;

mtgb::Direct3DResource::Direct3DResource(WindowContext _windowContext)
	: WindowContextResource(_windowContext)
	, pRenderTargetView_{nullptr}
	, pDepthStencil_{nullptr}
	, pDepthStencilView_{nullptr}
{
	// DirectX11Manager にアクセス
	auto& dx11Manager = Game::System<DirectX11Manager>();

	DXGIResource& dxgi = Game::System<WindowContextResourceManager>().Get<DXGIResource>(_windowContext);

	// レンダーターゲットビューを作成
	dx11Manager.CreateRenderTargetView(dxgi.pSwapChain1_.Get(), pRenderTargetView_.ReleaseAndGetAddressOf());

	// ビューポートを作成
	const Vector2Int SCREEN_SIZE{ Game::System<WindowManager>().GetWindowSize(_windowContext)};
	dx11Manager.CreateViewport(SCREEN_SIZE, viewPort_);

	// 深度ステンシルと深度ステンシルビューを作成
	dx11Manager.CreateDepthStencilAndDepthStencilView(SCREEN_SIZE, pDepthStencil_.ReleaseAndGetAddressOf(), pDepthStencilView_.ReleaseAndGetAddressOf());	
}

mtgb::Direct3DResource::~Direct3DResource()
{
	Release();
}

void mtgb::Direct3DResource::SetResource()
{
	DirectX11Manager& dx11Manager{ Game::System<DirectX11Manager>() };
	dx11Manager.ChangeViewport(viewPort_);
	dx11Manager.ChangeRenderTargets(pRenderTargetView_, pDepthStencilView_);
}

void mtgb::Direct3DResource::Reset()
{
	// リソースを解放する
	Release();
}

void mtgb::Direct3DResource::OnResize(UINT _width, UINT _height)
{
	// DirectX11Manager にアクセス
	auto& dx11Manager = Game::System<DirectX11Manager>();

	DXGIResource& dxgi = Game::System<WindowContextResourceManager>().Get<DXGIResource>(windowContext_);

	// レンダーターゲットビューを作成
	dx11Manager.CreateRenderTargetView(dxgi.pSwapChain1_.Get(), pRenderTargetView_.ReleaseAndGetAddressOf());

	// ビューポートを作成
	const Vector2Int SCREEN_SIZE{ static_cast<int>(_width),static_cast<int>(_height) };
	dx11Manager.CreateViewport(SCREEN_SIZE, viewPort_);

	// 深度ステンシルと深度ステンシルビューを作成
	dx11Manager.CreateDepthStencilAndDepthStencilView(SCREEN_SIZE, pDepthStencil_.ReleaseAndGetAddressOf(), pDepthStencilView_.ReleaseAndGetAddressOf());
}

void mtgb::Direct3DResource::Release()
{
	pDepthStencil_.Reset();
	pDepthStencilView_.Reset();
	pRenderTargetView_.Reset();
}

const D3D11_VIEWPORT& mtgb::Direct3DResource::GetViewport()
{
	return viewPort_;
}


