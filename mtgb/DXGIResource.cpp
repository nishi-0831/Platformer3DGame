#include "DXGIResource.h"
#include "ReleaseUtility.h"
#include "WindowContextUtil.h"
#include "Debug.h"
#include "MTImGui.h"
#include <processthreadsapi.h>
using namespace mtgb;

mtgb::DXGIResource::DXGIResource(WindowContext _windowContext)
	: WindowContextResource(_windowContext)
	, pSwapChain1_{nullptr}
	, pOutput_{nullptr}
	, pDXGISurface_{nullptr}
	, monitorInfo_{}
	, isMultiMonitor_{true}
	, isBorderlessWindow{true}
	, name_{"UnknownWindow"}
	, outputDesc_{}
{
	if (_windowContext == WindowContext::First)
	{
		name_ = "FirstWindowDXGI";
	}
	else if (_windowContext == WindowContext::Second)
	{
		name_ = "SecondWindowDXGI";
	}

	// DirectX11Manager にアクセスしてリソースを作成
	auto& dx11Manager = Game::System<DirectX11Manager>();

	HWND hWnd = WinCtxRes::GetHWND(_windowContext);

	// マルチモニター対応かどうか
	
	if (isMultiMonitor_)
	{	
		
		std::optional<MonitorInfo> optMonitorInfo = dx11Manager.AssignAvailableMonitor(pOutput_.ReleaseAndGetAddressOf());
		if (optMonitorInfo)
		{
			monitorInfo_ = *optMonitorInfo;
		}

		// ウィンドウサイズの変更時に利用するためにDescを取得
		HRESULT hResult = pOutput_->GetDesc(&outputDesc_);
		massert(SUCCEEDED(hResult)
			&& "GetDesc に失敗 @DXGIResource::Initialize");

		// ボーダレスウィンドウの場合、スワップチェーンのバッファサイズを変えるだけなので、Outputは不要。
		// ID3D11Deviceの作成に使用したIDXGIAdapterと、IDXGIOutputの列挙に使用したIDXGIAdapterが異なると、スワップチェーンの作成に失敗する。
		// 現状はID3D11Deviceは一つだけ作成しているので、IDXGIAdapterが異なる可能性を考慮してOutputは解放する
		if (isBorderlessWindow)
		{
			pOutput_.Reset();
		}
	}
	else 
	{
		pOutput_ = nullptr;
	}

	// スワップチェーンを作成(pOutput_がnullptrでもデフォルトのIDXGIAdapterから作成される)
	dx11Manager.CreateSwapChain(hWnd, pOutput_.Get(), pSwapChain1_.ReleaseAndGetAddressOf());

	// サーフェスとして作成
	dx11Manager.CreateDXGISurface(pSwapChain1_.Get(), pDXGISurface_.ReleaseAndGetAddressOf());
}

mtgb::DXGIResource::~DXGIResource()
{
	Release();
}

void DXGIResource::SetResource()
{
	Game::System<DirectX11Manager>().ChangeSwapChain(pSwapChain1_);
}

void mtgb::DXGIResource::Update()
{
	MTImGui::Instance().DirectShow([this]
		{
			// モニター(DXGIOutput)の情報
			ImGui::PushID(&monitorInfo_);
			ImGui::LabelText("adapterIndex", "%d", monitorInfo_.adapterIndex);
			ImGui::LabelText("outputIndex", "%d", monitorInfo_.outputIndex);
			TypeRegistry::Instance().CallFunc(&monitorInfo_.desc, "OutputDesc");
			ImGui::PopID();
			ImGui::Separator();
		}, name_.c_str(), ShowType::Settings);
}

void mtgb::DXGIResource::Reset()
{
	pDXGISurface_.Reset();
}

void mtgb::DXGIResource::OnResize(UINT _width, UINT _height)
{
	HRESULT hResult = pSwapChain1_->ResizeBuffers(
		0, _width, _height, DXGI_FORMAT_UNKNOWN, 0);
	massert(SUCCEEDED(hResult) && "ResizeBuffers に失敗 @DXGIResource::OnResize");

	// 再取得
	Game::System<DirectX11Manager>().CreateDXGISurface(pSwapChain1_.Get(), pDXGISurface_.ReleaseAndGetAddressOf());
}

void mtgb::DXGIResource::Release()
{
	// SwapChain はフルスクリーンモードでは削除できないらしい
	if (pSwapChain1_)
	{
		pSwapChain1_->SetFullscreenState(false, nullptr);
	}
	pSwapChain1_.Reset();
	pOutput_.Reset();
	pDXGISurface_.Reset();
}

void mtgb::DXGIResource::SwapMonitorInfo(DXGIResource& _other)
{
	std::swap(monitorInfo_, _other.monitorInfo_);
	std::swap(outputDesc_, _other.outputDesc_);
}


