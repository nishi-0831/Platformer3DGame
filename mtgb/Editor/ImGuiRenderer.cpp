#include "ImGuiRenderer.h"
#include "Core/Game.h"
#include "DirectX11Draw.h"
#include "DirectX11Manager.h"
#include "WindowContext/WindowContext.h"
#include "WindowContext/WindowContextUtil.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"
#include "Screen.h"
#include "Editor/ImGuiUtil.h"
#include <filesystem>

namespace fs = std::filesystem;
mtgb::ImGuiRenderer::ImGuiRenderer()
	: winWidth_ { 800 }
	, winHeight_ { 600 }
	, pRenderTargetView_ { nullptr }
	, pSRV_ { nullptr }
	, pSRVTexture_ { nullptr }
	, pTexture_ { nullptr }
	, pDepthStencil_ { nullptr }
	, pDepthStencilView_ { nullptr }
	, viewport_ {}
{
}
mtgb::ImGuiRenderer::~ImGuiRenderer() {}

void mtgb::ImGuiRenderer::Initialize()
{
	Vector2Int size = Game::System<Screen>().GetSize();
	winWidth_		= static_cast<UINT>(size.x);
	winHeight_		= static_cast<UINT>(size.y);

	IMGUI_CHECKVERSION();

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;
	io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;

	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

	ImFont* font =
		io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\meiryo.ttc", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	IM_ASSERT(font != nullptr);

	ImGui::StyleColorsDark();

	ImGui::SetCurrentContext(ImGui::GetCurrentContext());

	ImGui_ImplWin32_Init(WinCtxRes::GetHWND(WindowContext::FIRST));
	ComPtr<ID3D11Device> device			= mtgb::DirectX11Draw::pDevice_;
	ComPtr<ID3D11DeviceContext> context = mtgb::DirectX11Draw::pContext_;
	ImGui_ImplDX11_Init(device.Get(), context.Get());

	ImGui::LoadIniSettingsFromDisk("Assets/imgui.ini");
	CreateD3DResources();
	char defaultCurrentDirectory[MAX_PATH] {};
	GetCurrentDirectory(MAX_PATH, defaultCurrentDirectory);

	fs::path current_dir = fs::current_path();
	Game::OnExit(
		[]()
		{
			ImGui::SaveIniSettingsToDisk("Assets/imgui.ini");
		}
	);
}

void mtgb::ImGuiRenderer::Update() {}

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

void mtgb::ImGuiRenderer::Begin(const char* _str, bool* _isOpen, WindowFlags _flag)
{
	ImGuiWindowFlags flags = 0;
	if (_flag & WindowFlag::NO_MOVE_WHEN_HOVERED)
	{
		if (mtgb::ImGuiUtil::IsMouseInWindow(_str))
		{
			flags |= ImGuiWindowFlags_NoMove;
		}
	}
	if (_flag & WindowFlag::NO_SCROLL)
	{
		flags |= ImGuiWindowFlags_NoScrollWithMouse;
	}
	ImGui::Begin(_str, _isOpen, flags);
}

void mtgb::ImGuiRenderer::SetImGuizmoRenderTargetView()
{
	Game::System<DirectX11Manager>().ChangeRenderTargets(pRenderTargetView_, pDepthStencilView_);
}

void mtgb::ImGuiRenderer::Draw() {}

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

void mtgb::ImGuiRenderer::End()
{
	ImGui::End();
}
void mtgb::ImGuiRenderer::Release()
{
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

void mtgb::ImGuiRenderer::OnResize(UINT _width, UINT _height)
{
	CreateD3DResources();
}

void mtgb::ImGuiRenderer::CreateD3DResources()
{
	ComPtr<ID3D11Device> device			= mtgb::DirectX11Draw::pDevice_;
	ComPtr<ID3D11DeviceContext> context = mtgb::DirectX11Draw::pContext_;

	Game::System<DirectX11Manager>().CreateViewport(
		{ static_cast<int>(winWidth_), static_cast<int>(winHeight_) },
		viewport_
	);

	// RTV用テクスチャ作成
	D3D11_TEXTURE2D_DESC desc { .Width		= winWidth_,
								.Height		= winHeight_,
								.MipLevels	= 1,
								.ArraySize	= 1,
								.Format		= DXGI_FORMAT_R8G8B8A8_UNORM,
								.SampleDesc = { .Count = 1 },
								.Usage		= D3D11_USAGE_DEFAULT,
								.BindFlags	= D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE };

	HRESULT hResult = device->CreateTexture2D(&desc, nullptr, pTexture_.ReleaseAndGetAddressOf());
	massert(SUCCEEDED(hResult) && "CreateTexture2Dに失敗 @ImGuiRenderer::CreateD3DResources");

	hResult = device->CreateRenderTargetView(pTexture_.Get(), nullptr, pRenderTargetView_.ReleaseAndGetAddressOf());
	massert(SUCCEEDED(hResult) && "CreateRenderTargetViewに失敗 @ImGuiRenderer::CreateD3DResources");

	hResult = device->CreateShaderResourceView(pTexture_.Get(), nullptr, pSRV_.ReleaseAndGetAddressOf());
	massert(SUCCEEDED(hResult) && "CreateShaderResourceViewに失敗 @ImGuiRenderer::CreateD3DResources");

	// 深度ステンシルと深度ステンシルビューを作成
	Game::System<DirectX11Manager>().CreateDepthStencilAndDepthStencilView(
		Vector2Int(static_cast<int>(winWidth_), static_cast<int>(winHeight_)),
		pTexture_.GetAddressOf(),
		pDepthStencilView_.ReleaseAndGetAddressOf()
	);
}
