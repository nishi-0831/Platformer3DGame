#include "Direct2DResource.h"
#include "Direct2D/Direct2D.h"
#include <d2d1helper.h>
#include "IncludingWindows.h"
#include "Game.h"
#include "WindowContext.h"
#include "WindowContextResource.h"
#include "Direct3DResource.h"
#include "DXGIResource.h"
using namespace mtgb;

mtgb::Direct2DResource::Direct2DResource(WindowContext _windowContext)
	: WindowContextResource(_windowContext)
	, pRenderTarget_{nullptr}
	, pD2DBrush_{nullptr}
{
	// DXGI,Direct3Dのリソースを取得
	DXGIResource& dxgiResource = Game::System<WindowContextResourceManager>().Get<DXGIResource>(_windowContext);
	Direct3DResource& d3dResource = Game::System<WindowContextResourceManager>().Get<Direct3DResource>(_windowContext);

	// Direct2DのComオブジェクト作成に使う
	Direct2D& direct2D = Game::System<Direct2D>();
 
	// レンダーターゲット、ブラシを作成
	direct2D.CreateD2DRenderTarget(dxgiResource.pDXGISurface_.Get(), pRenderTarget_.ReleaseAndGetAddressOf());

	// TODO: ブラシの色指定をハードコーディングせずに、外部ファイル、プログラムから指定できるようにする
	direct2D.CreateSolidColorBrush(D2D1::ColorF::White, pRenderTarget_.Get(), pD2DBrush_.ReleaseAndGetAddressOf());
}

mtgb::Direct2DResource::~Direct2DResource()
{
	Release();
}

void mtgb::Direct2DResource::Reset()
{
	Release();
}

void mtgb::Direct2DResource::OnResize(UINT _width, UINT _height)
{
	Release();
	
	// DXGI,Direct3Dのリソースを取得
	DXGIResource& dxgiResource = Game::System<WindowContextResourceManager>().Get<DXGIResource>(windowContext_);
	Direct3DResource& d3dResource = Game::System<WindowContextResourceManager>().Get<Direct3DResource>(windowContext_);

	// Direct2DのComオブジェクト作成に使う
	Direct2D& direct2D = Game::System<Direct2D>();

	// レンダーターゲット、ブラシを作成
	direct2D.CreateD2DRenderTarget(dxgiResource.pDXGISurface_.Get(), pRenderTarget_.ReleaseAndGetAddressOf());

	// TODO: ブラシの色指定をハードコーディングせずに、外部ファイル、プログラムから指定できるようにする
	direct2D.CreateSolidColorBrush(D2D1::ColorF::White, pRenderTarget_.Get(), pD2DBrush_.ReleaseAndGetAddressOf());
}

void mtgb::Direct2DResource::SetResource()
{
	// ブラシ、レンダーターゲットをセット
	Game::System<Direct2D>().ChangeRenderTarget(pD2DBrush_, pRenderTarget_);
}

void mtgb::Direct2DResource::Release()
{
	pD2DBrush_.Reset();
	pRenderTarget_.Reset();
}

