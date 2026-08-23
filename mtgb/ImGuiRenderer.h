#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include "ImGui/imgui.h"

#include "ISystem.h"
#include "Editor/ImGuizmoManipulator.h"
#include "Editor/ImGuiEditorCamera.h"
using Microsoft::WRL::ComPtr;
struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct ID3D11Texture2D;
struct ID3D11DepthStencilView;

namespace mtgb
{
	class GameObject;
	class Transform;
	struct Vector3;

	class ImGuiRenderer final : public ISystem
	{
	  public:
		using WindowFlags = int;
		enum WindowFlag
		{
			NONE,
			NO_MOVE_WHEN_HOVERED, // マウスカーソルがウィンドウ内にあるとき移動禁止
			NO_SCROLL
		};

	  public:
		ImGuiRenderer();
		~ImGuiRenderer();
		void Initialize() override;
		void Update() override;
		/// <summary>
		/// ImGuiのフレームを開始
		/// </summary>
		void BeginFrame();
		/// <summary>
		/// ImGuizmoのフレームを開始
		/// </summary>
		void BeginImGuizmoFrame();
		/// <summary>
		/// ImGuiのウィンドウを開始
		/// </summary>
		/// <param name="_str">ウィンドウ名</param>
		/// <param name="_isOpen"></param>
		/// <param name="_flag">表示フラグ</param>
		void Begin(const char* _str, bool* _isOpen = NULL, WindowFlags _flag = 0);
		/// <summary>
		/// ImGuiのウィンドウを終了
		/// </summary>
		void End();
		void Draw();
		/// <summary>
		/// ImGuiのフレームを終了
		/// </summary>
		void EndFrame();
		void SetDrawList();

		/// <summary>
		/// ImGuizmoウィンドウを描画するためにRTVをセット
		/// </summary>
		void SetImGuizmoRenderTargetView();
		/// <summary>
		/// シーンビューを描画
		/// </summary>
		void RenderSceneView();
		void Release() override;
		/// <summary>
		/// ImGuiが保持しているリソースを解放
		/// </summary>
		void ResetComPtrs();
		/// <summary>
		/// ウィンドウリサイズ時の処理
		/// </summary>
		/// <param name="width">新しい幅</param>
		/// <param name="height">新しい高さ</param>
		void OnResize(UINT _width, UINT _height);

		D3D11_VIEWPORT GetViewport()
		{
			return viewport_;
		}

	  private:
		void CreateD3DResources();
		UINT winWidth_, winHeight_;

		// シーンビューの描画に使用するオブジェクト

		ComPtr<ID3D11RenderTargetView> pRenderTargetView_;
		ComPtr<ID3D11ShaderResourceView> pSRV_;
		ComPtr<ID3D11Texture2D> pSRVTexture_;
		ComPtr<ID3D11Texture2D> pTexture_;
		ComPtr<ID3D11Texture2D> pDepthStencil_;
		ComPtr<ID3D11DepthStencilView> pDepthStencilView_;
		D3D11_VIEWPORT viewport_;
	};
} // namespace mtgb