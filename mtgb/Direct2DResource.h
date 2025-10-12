#pragma once
#include "WindowContextResource.h"
#include "WindowContextResourceManager.h"

#include "Direct3DResource.h"
#include "MTAssert.h"
#include "Direct2D/Direct2D.h"
#include "DirectWrite.h"
#include "Game.h"
#include "ISystem.h"
#include <d2d1.h>
#include "WindowContext.h"
#include <map>
#include <typeindex>

typedef struct HWND__* HWND;

namespace mtgb
{

	class Direct2DResource : public WindowContextResource
	{
		friend class DirectWrite;
	public:
		Direct2DResource();
		~Direct2DResource();
		//Direct2DResource(const Direct2DResource& other);
		void Initialize(WindowContext _windowContext) override;
		void Reset() override;
		void OnResize(WindowContext _windowContext, UINT _width, UINT _height) override;
		void SetResource() override;
		void Release() override;
	private:

		ComPtr<ID2D1RenderTarget> pRenderTarget_;
		ComPtr<ID2D1SolidColorBrush> pD2DBrush_;

		// WindowContextResource ����Čp������܂���
		WindowContextResource* Clone() const override;

	};
}