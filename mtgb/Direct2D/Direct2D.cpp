#include "Direct2D.h"
#include "MTAssert.h"
#include "DirectX11Draw.h"
#include "ReleaseUtility.h"

#include <d3d11_1.h>
#include <comdef.h>
#include <d3d11.h>
#include <d2d1.h>
#include "IncludingWindows.h"
// #include <dwrite.h>  // direct write のやつ

#pragma comment(lib, "d3d11.lib")
// #pragma comment(lib,"d3dx11.lib")
// #pragma comment(lib,"dwrite.lib")
#pragma comment(lib, "d2d1.lib")

ComPtr<ID2D1Factory> mtgb::Direct2D::pD2DFactory_ { nullptr };
ComPtr<ID2D1SolidColorBrush> mtgb::Direct2D::pDefD2DBrush_ { nullptr };
ComPtr<ID2D1RenderTarget> mtgb::Direct2D::pDefRenderTarget_ { nullptr };

mtgb::Direct2D::~Direct2D()
{
	Release();
}

void mtgb::Direct2D::Initialize()
{
	InitializeCommonResource();
}

void mtgb::Direct2D::InitializeCommonResource()
{
	// ファクトリー作成
	HRESULT hResult = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, pD2DFactory_.ReleaseAndGetAddressOf());

	massert(SUCCEEDED(hResult) && "D2D1CreateFactoryに失敗 @Direct2D::Initialize");
}

void mtgb::Direct2D::CreateD2DRenderTarget(IDXGISurface* _pIdxgiSurface, ID2D1RenderTarget** _ppRenderTarget)
{
	// レンダーターゲットのプロパティ設定書く
	D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties = D2D1::RenderTargetProperties(

		D2D1_RENDER_TARGET_TYPE_DEFAULT, // レンダリングモード。DEFAULTだと使用可能ならGPU、それ以外はCPUでレンダリング

		D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_IGNORE), // ピクセルの形式、アルファ値

		0, // x方向のdpi
		0, // y方向のdpi

		D2D1_RENDER_TARGET_USAGE_NONE, // 何かしらの利用方法を設定

		D2D1_FEATURE_LEVEL_DEFAULT // 最小の機能レベルを指定

	);

	// レンダーターゲット作成
	HRESULT hResult =
		pD2DFactory_->CreateDxgiSurfaceRenderTarget(_pIdxgiSurface, renderTargetProperties, _ppRenderTarget);

	// massert(SUCCEEDED(hResult) && "CreateDxgiSurfaceRenderTargetに失敗 @Direct2D::CreateDXGISurfaceRenderTarget");
	if (FAILED(hResult))
	{
		_com_error error(hResult);
		LPCTSTR message = error.ErrorMessage();
		std::string str = mtgb::MultiToUTF8(message);
		MessageBoxW(NULL, mtgb::UTF8ToWide(str.c_str()).c_str(), L"f", MB_YESNOCANCEL | MB_ICONSTOP | MB_SYSTEMMODAL);
	}
}

void mtgb::Direct2D::CreateSolidColorBrush(
	const D2D1::ColorF& _color,
	ID2D1RenderTarget* _pRenderTarget,
	ID2D1SolidColorBrush** _ppD2DBrush
)
{
	// ブラシ作成
	HRESULT hResult = _pRenderTarget->CreateSolidColorBrush(_color, _ppD2DBrush);

	massert(SUCCEEDED(hResult) && "CreateSolidColorBrushに失敗 @Direct2D::CreateSolidColorBrush");
}

void mtgb::Direct2D::ChangeRenderTarget(
	ComPtr<ID2D1SolidColorBrush> _pD2DBrush,
	ComPtr<ID2D1RenderTarget> _pRenderTarget
)
{
	pDefD2DBrush_	  = _pD2DBrush;
	pDefRenderTarget_ = _pRenderTarget;
}

void mtgb::Direct2D::Update() {}

void mtgb::Direct2D::Draw() {}

void mtgb::Direct2D::Release()
{
	pD2DFactory_.Reset();

	pDefD2DBrush_.Reset();
	pDefRenderTarget_.Reset();
}

void mtgb::Direct2D::Reset()
{
	pDefD2DBrush_.Reset();
	pDefRenderTarget_.Reset();
}

D2D1_SIZE_F mtgb::Direct2D::GetRenderTargetSize()
{
	return pDefRenderTarget_->GetSize();
}
