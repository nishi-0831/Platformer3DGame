#pragma once
#include <tuple>
#include <map>
#include <typeindex>
#include <d3d11.h>
#include "WindowContext.h"

struct IDXGISwapChain1;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct ID2D1RenderTarget;

namespace mtgb
{
	/// <summary>
	/// ウィンドウごとに固有のリソースのインターフェース
	/// </summary>
	class WindowContextResource
	{
	public:
		virtual ~WindowContextResource() = default;
		/// <summary>
		/// ウィンドウを変更する際に固有のリソースをセットする
		/// </summary>
		virtual void SetResource() = 0;

		/// <summary>
		/// ウィンドウのサイズが変更される前の解放処理
		/// </summary>
		virtual void Reset() {}
		/// <summary>
		/// ウィンドウのサイズが変更された後の処理
		/// </summary>
		/// <param name="_width">幅</param>
		/// <param name="_height">高さ</param>
		virtual void OnResize(UINT _width, UINT _height) {}
		virtual void Release() = 0;

		/// <summary>
		/// 更新処理
		/// </summary>
		virtual void Update() {}
		/// <summary>
		/// ウィンドウの識別子を返す
		/// </summary>
		/// <returns></returns>
		WindowContext GetWindowContext() const { return windowContext_; }
	protected:
		explicit WindowContextResource(WindowContext _windowContext)
			: windowContext_(_windowContext) {}
		
		WindowContext windowContext_;
		
		// コピーコンストラクタとコピー代入演算子を削除
		WindowContextResource(const WindowContextResource&) = delete;
		WindowContextResource& operator=(const WindowContextResource&) = delete;
	};
}