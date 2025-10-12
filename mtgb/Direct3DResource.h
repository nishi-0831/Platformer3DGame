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
		Direct3DResource();
		~Direct3DResource();
		//Direct3DResource(const Direct3DResource& other);
		void Initialize(WindowContext _windowContext) override;
		void SetResource() override;
		void Reset() override;
		void OnResize(WindowContext _windowContext, UINT _width, UINT _height) override;
		void Release() override;

		const D3D11_VIEWPORT& GetViewport();
	private:
		
		D3D11_VIEWPORT viewPort_;
		
		ComPtr<ID3D11RenderTargetView> pRenderTargetView_;
		ComPtr<ID3D11Texture2D> pDepthStencil_;
		ComPtr<ID3D11DepthStencilView> pDepthStencilView_;


		// WindowContextResource ����Čp������܂���
		Direct3DResource* Clone() const override;

	};
}