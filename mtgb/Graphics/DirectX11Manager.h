#pragma once
#include "Core/ISystem.h"
#include <string>
#include <wrl/client.h>
#include "Math/Vector2Int.h"
#include <dxgi1_2.h>
#include <vector>
#include <optional>
#include "Window/MonitorInfo.h"

#pragma comment(lib, "d3d11.lib") // DirectX11のライブラリ
#pragma comment(lib, "dxgi.lib")

using Microsoft::WRL::ComPtr;
struct D3D11_INPUT_ELEMENT_DESC;
struct CD3D11_RASTERIZER_DESC;
struct D3D11_VIEWPORT;
struct IDXGISwapChain1;
struct IDXGISurface;
struct IDXGIOutput;
struct ID3D11RenderTargetView;
struct ID3D11Texture2D;
struct ID3D11DepthStencilView;
typedef struct HWND__* HWND;

namespace mtgb
{
	class DirectX11Manager : public ISystem
	{
	  public:
		DirectX11Manager();
		~DirectX11Manager();

		void Initialize() override;
		void Update() override;

		void InitializeCommonResources();
		void CreateDXGISurface(IDXGISwapChain1* _pSwapChain1, IDXGISurface** _ppDxgiSurface);

		void CreateSwapChain(HWND _hWnd, IDXGIOutput* _pOutput, IDXGISwapChain1** _ppSwapChain1);
		void CreateRenderTargetView(IDXGISwapChain1* _pSwapChain1, ID3D11RenderTargetView** _ppRenderTargetView);
		void CreateViewport(const Vector2Int& _size, D3D11_VIEWPORT& _viewport);
		void CreateDepthStencilAndDepthStencilView(
			const Vector2Int& _bufSize,
			ID3D11Texture2D** _ppDepthStencil,
			ID3D11DepthStencilView** _ppDepthStencilView
		);

		void ChangeViewport(const D3D11_VIEWPORT& _viewport);
		void ChangeRenderTargets(
			ComPtr<ID3D11RenderTargetView> _pRenderTargetView,
			ComPtr<ID3D11DepthStencilView> _pDepthStencilView
		);
		void ChangeSwapChain(ComPtr<IDXGISwapChain1> _pSwapChain1);

		void ClearState();
		/// <summary>
		/// ClearState後に必要なデフォルト状態を再設定
		/// </summary>
		void SetDefaultStates();

		/// <summary>
		/// 利用可能なモニターを自動で割り当てる
		/// </summary>
		/// <param name="ppOutput">これにモニターを割り当てる</param>
		/// <returns>割り当てられたモニターのインデックス、失敗時は -1</returns>
		std::optional<MonitorInfo> AssignAvailableMonitor(IDXGIOutput** _ppOutput);

		int GetAvailableMonitorCount() const;

		void Release() override;

	  private:
		std::vector<DXGI_ADAPTER_DESC1> adaptersDesc_;
		/// <summary>
		/// 利用可能なモニターを列挙する
		/// </summary>
		void EnumAvailableMonitors();
	};
} // namespace mtgb
