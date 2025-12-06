#pragma once
#include "WindowContextResource.h"
#include "WindowContextResourceManager.h"
#include "Game.h"
#include "ISystem.h"
#include "DirectX11Manager.h"
#include "MTAssert.h"
#include <dxgi1_2.h>
#include "WindowContext.h"
#include <map>
#include <typeindex>
#include <string>
#include "MonitorInfo.h"
typedef struct HWND__* HWND;

namespace mtgb
{

	class DXGIResource : public WindowContextResource
	{
	public:
		explicit DXGIResource(WindowContext _windowContext);
		~DXGIResource();
		
		void SetResource() override;
		void Update() override;
		void Reset() override;
		void OnResize(UINT _width, UINT _height) override;
		void Release() override;

		/// <summary>
		/// 割り当てられたモニターの座標を返す
		/// </summary>
		RECT GetAssignedMonitorRect() const { return outputDesc_.DesktopCoordinates; }

		ComPtr<IDXGISwapChain1> pSwapChain1_;
		ComPtr<IDXGIOutput> pOutput_;
		ComPtr<IDXGISurface> pDXGISurface_;

		/// <summary>
		/// モニター情報を交換する
		/// </summary>
		/// <param name="_other"></param>
		void SwapMonitorInfo(DXGIResource& _other);
	private:
		
		MonitorInfo monitorInfo_;
		bool isMultiMonitor_; // マルチ
		bool isBorderlessWindow;
		std::vector<DXGI_MODE_DESC> modeList_;
		std::string name_;
		DXGI_OUTPUT_DESC outputDesc_;

		// コピーコンストラクタとコピー代入演算子を削除
		DXGIResource(const DXGIResource&) = delete;
		DXGIResource& operator=(const DXGIResource&) = delete;

	};
}