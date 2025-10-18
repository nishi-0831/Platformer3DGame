#include "ImGuiRenderer.h"
#include "Game.h"
#include "DirectX11Draw.h"
#include "DirectX11Manager.h"
#include "WindowContextUtil.h"
#include "../ImGui/imgui_impl_win32.h"
#include "../ImGui/imgui_impl_dx11.h"
#include "Screen.h"
#include "ImGuiUtil.h"
namespace
{
	float titleBarHeight;
	const unsigned long long NULLID = 0;
	const char* WINDOWNAME_GAME_VIEW = "Game View";
}





mtgb::ImGuiRenderer::ImGuiRenderer()
	:pRenderTargetView_{nullptr}
	,pSRV_{nullptr}
	,pSRVTexture_{nullptr}
	,pTexture_{nullptr}
	,pDepthStencil_{nullptr}
	,pDepthStencilView_{nullptr}
	,gameViewRectValid_{false}
	
{
	
}
mtgb::ImGuiRenderer::~ImGuiRenderer()
{
	SAFE_DELETE(pEditorCamera_);
	SAFE_DELETE(pManipulator_);
}

void mtgb::ImGuiRenderer::Initialize()
{
	Vector2Int size = Game::System<Screen>().GetSize();
	winWidth_ = static_cast<UINT>(size.x);
	winHeight_ = static_cast<UINT>(size.y);

	IMGUI_CHECKVERSION();

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();(void)io;

	io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;
	io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;

	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	
	
	ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\meiryo.ttc", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	IM_ASSERT(font != nullptr);

	//ImGui_ImplWin32_EnableDpiAwareness();
	//float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));
	//// Setup scaling
	//ImGuiStyle& style = ImGui::GetStyle();
	//style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
	//style.FontScaleDpi = main_scale;        // Set initial font scale. (using 

	//io.ConfigDpiScaleFonts = true;
	//io.ConfigDpiScaleViewports = true;
	
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	ImGui::SetCurrentContext(ImGui::GetCurrentContext());

	ImGui_ImplWin32_Init(WinCtxRes::GetHWND(WindowContext::First));
	const auto& ctx = ImGui::GetCurrentContext();
	ComPtr<ID3D11Device> device = mtgb::DirectX11Draw::pDevice_;
	ComPtr<ID3D11DeviceContext> context = mtgb::DirectX11Draw::pContext_;
	ImGui_ImplDX11_Init(device.Get(), context.Get());

	pManipulator_ = new ImGuizmoManipulator();
	pEditorCamera_ = new ImGuiEditorCamera();
	pEditorCamera_->SetViewPort(viewport_);
	
	CreateD3DResources();
}

void mtgb::ImGuiRenderer::Update()
{
	pEditorCamera_->UpdateState();
}

void mtgb::ImGuiRenderer::BeginFrame()
{
	ImGui::SetCurrentContext(ImGui::GetCurrentContext());
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	
}
void mtgb::ImGuiRenderer::BeginImGuizmoFrame()
{
	ImGuizmo::BeginFrame();
}

void mtgb::ImGuiRenderer::SetWindowName(const char* _name)
{
	pEditorCamera_->SetWindowName(_name);
}

void mtgb::ImGuiRenderer::Begin(const char* _str, bool* _isOpen, WindowFlag _flag)
{
	ImGuiWindowFlags flags = 0;
	if (_flag == WindowFlag::NoMoveWhenHovered)
	{
		if (mtgb::ImGuiUtil::IsMouseInWindow(_str))
		{
			flags |= ImGuiWindowFlags_NoMove;
		}
	}
	ImGui::Begin(_str, _isOpen, flags);
}


void mtgb::ImGuiRenderer::SetImGuizmoRenderTargetView()
{
	//ID3D11ShaderResourceView* nullSRV[16]{};
	//// 全スロットのSRVをクリア
	//DirectX11Draw::pContext_->PSSetShaderResources(0, 16, nullSRV);

	//DirectX11Draw::pContext_->PSSetShaderResources(0, 0, pSRV_.GetAddressOf());
	//Game::System<DirectX11Manager>().ChangeViewport(viewport_);
	Game::System<DirectX11Manager>().ChangeRenderTargets(pRenderTargetView_,pDepthStencilView_);
}
void mtgb::ImGuiRenderer::SetGameViewCamera()
{
	pEditorCamera_->SetCamera();
}
void mtgb::ImGuiRenderer::Draw()
{

}

void mtgb::ImGuiRenderer::EndFrame()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

void mtgb::ImGuiRenderer::SetDrawList()
{
	ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
}

void mtgb::ImGuiRenderer::RenderSceneView()
{
	
	ImGui::Image((void*)pSRV_.Get(), ImVec2(static_cast<float>(winWidth_), static_cast<float>(winHeight_)));
}

void mtgb::ImGuiRenderer::UpdateGameViewRect()
{
	gameViewPos_ = ImGui::GetWindowPos();
	gameViewSize_ = ImGui::GetWindowSize();
	gameViewRectValid_ = true;
}

void mtgb::ImGuiRenderer::End()
{
	ImGui::End();
}
void mtgb::ImGuiRenderer::Release()
{
	SAFE_DELETE(pManipulator_);
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void mtgb::ImGuiRenderer::ResetComPtrs()
{
	// ImGuiが保持しているリソース解放
	ImGui_ImplDX11_InvalidateDeviceObjects();

	pSRV_.Reset();
	pRenderTargetView_.Reset();
	pDepthStencilView_.Reset();
	pDepthStencil_.Reset();
	pTexture_.Reset();

}

void mtgb::ImGuiRenderer::OnResize(UINT width, UINT height)
{
	CreateD3DResources();
    // ウィンドウサイズを更新
    /*winWidth_ = width;
    winHeight_ = height;*/
    
    // GameView の矩形情報をリセット
    gameViewRectValid_ = false;
    
    // ImGuiのディスプレイサイズを更新
    /*ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(static_cast<float>(width), static_cast<float>(height));*/
    
    // ImGuiのリソースを再作成する必要がある場合
    // ImGui_ImplDX11_InvalidateDeviceObjects();
    // ImGui_ImplDX11_CreateDeviceObjects();
}



void mtgb::ImGuiRenderer::CreateD3DResources()
{
	ComPtr<ID3D11Device> device = mtgb::DirectX11Draw::pDevice_;
	ComPtr<ID3D11DeviceContext> context = mtgb::DirectX11Draw::pContext_;

	Game::System<DirectX11Manager>().CreateViewport({ static_cast<int>(winWidth_) ,static_cast<int>(winHeight_) }, viewport_);

	// RTV用テクスチャ作成
	D3D11_TEXTURE2D_DESC desc
	{
		.Width = winWidth_,
		.Height = winHeight_,
		.MipLevels = 1,
		.ArraySize = 1,
		.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
		.SampleDesc = { .Count = 1 },
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE
	};
	
	HRESULT hResult = device->CreateTexture2D(&desc, nullptr, pTexture_.ReleaseAndGetAddressOf());
	massert(SUCCEEDED(hResult) && "CreateTexture2Dに失敗 @ImGuiRenderer::CreateD3DResources");

	hResult = device->CreateRenderTargetView(pTexture_.Get(), nullptr, pRenderTargetView_.ReleaseAndGetAddressOf());
	massert(SUCCEEDED(hResult) && "CreateRenderTargetViewに失敗 @ImGuiRenderer::CreateD3DResources");

	// SRV用テクスチャ作成
	//desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	//hResult = device->CreateTexture2D(&desc, nullptr, pTexture_.GetAddressOf());
	//massert(SUCCEEDED(hResult) && "CreateTexture2Dに失敗 @ImGuiRenderer::CreateD3DResources");

	hResult = device->CreateShaderResourceView(pTexture_.Get(), nullptr, pSRV_.ReleaseAndGetAddressOf());
	massert(SUCCEEDED(hResult) && "CreateShaderResourceViewに失敗 @ImGuiRenderer::CreateD3DResources");

	// 深度ステンシルと深度ステンシルビューを作成
	Game::System<DirectX11Manager>().CreateDepthStencilAndDepthStencilView(
		Vector2Int(static_cast<int>(winWidth_), static_cast<int>(winHeight_)),
		pTexture_.GetAddressOf(), pDepthStencilView_.ReleaseAndGetAddressOf());

	//ImGui_ImplDX11_CreateDeviceObjects();
}


