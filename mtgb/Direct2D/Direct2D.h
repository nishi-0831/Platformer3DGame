#pragma once
#include "cmtgb.h"
#include <d2d1.h>
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;


struct ID2D1Factory;
struct IDXGISurface;
struct ID2D1SolidColorBrush;
struct ID2D1RenderTarget;
struct IDXGISwapChain1;


namespace mtgb
{
	
	class Direct2D : public ISystem
	{
		friend class DirectWrite;
	public:
		~Direct2D();
		void Initialize() override;
		void InitializeCommonResource();
		void CreateD2DRenderTarget(IDXGISurface* pIDXGISurface, ID2D1RenderTarget** ppRenderTarget_);
		void CreateSolidColorBrush(const D2D1::ColorF& color, ID2D1RenderTarget* pRenderTarget, ID2D1SolidColorBrush** ppD2DBrush);

		void ChangeRenderTarget(ComPtr<ID2D1SolidColorBrush> pD2DBrush, ComPtr<ID2D1RenderTarget> pRenderTarget);

		void Update() override;
		void Draw();
		void Release();
		void Reset();
		D2D1_SIZE_F GetRenderTargetSize();
	private:
		static ComPtr<ID2D1Factory> pD2DFactory_;

		static ComPtr<ID2D1SolidColorBrush> pDefD2DBrush_;
		static ComPtr<ID2D1RenderTarget> pDefRenderTarget_;
	};
}
