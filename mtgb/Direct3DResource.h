#pragma once
#include "WindowContextResource.h"
#include "WindowContextResourceManager.h"
#include "DXGIResource.h"
#include "MTAssert.h"
#include "WindowContext.h"
#include <map>
#include <typeindex>

namespace mtgb
{

	class Direct3DResource : public WindowContextResource
	{
	public:
		explicit Direct3DResource(WindowContext _windowContext);
		~Direct3DResource();
		
		void SetResource() override;
		void Reset() override;
		void OnResize(UINT _width, UINT _height) override;
		void Release() override;

		const D3D11_VIEWPORT& GetViewport();
	private:
		D3D11_VIEWPORT viewPort_;
		
		ComPtr<ID3D11RenderTargetView> pRenderTargetView_;
		ComPtr<ID3D11Texture2D> pDepthStencil_;
		ComPtr<ID3D11DepthStencilView> pDepthStencilView_;

		// コピーコンストラクタとコピー代入演算子を削除
		Direct3DResource(const Direct3DResource&) = delete;
		Direct3DResource& operator=(const Direct3DResource&) = delete;

	};
}