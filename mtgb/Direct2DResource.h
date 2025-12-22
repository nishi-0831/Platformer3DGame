#pragma once
#include "WindowContextResource.h"

#include <d2d1.h>
#include "WindowContext.h"
#include "IncludingWindows.h"
#include <wrl/client.h>

typedef struct HWND__* HWND;

namespace mtgb
{

	class Direct2DResource : public WindowContextResource
	{
		friend class DirectWrite;
	public:
		explicit Direct2DResource(WindowContext _windowContext);
		~Direct2DResource();
		
		void Reset() override;
		void OnResize(UINT _width, UINT _height) override;
		void SetResource() override;
		void Release() override;
	private:
		ComPtr<ID2D1RenderTarget> pRenderTarget_;
		ComPtr<ID2D1SolidColorBrush> pD2DBrush_;

		// コピーコンストラクタとコピー代入演算子を削除
		Direct2DResource(const Direct2DResource&) = delete;
		Direct2DResource& operator=(const Direct2DResource&) = delete;

	};
}