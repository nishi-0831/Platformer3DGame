#pragma once
#include "WindowContextResource.h"
#include "Game.h"
#include "ISystem.h"
#include "DirectX11Manager.h"
#include "MTAssert.h"
#include "WindowContext.h"
#include "WindowManager.h"

#include "includingWindows.h"

#include <map>
#include <typeindex>

namespace mtgb
{
	// ウィンドウモード時の情報
	struct WindowModeInfo
	{
		WindowModeInfo();
		RECT windowedRect_;		//位置とサイズ
		LONG windowedStyle_;	// スタイル
		LONG windowedExStyle_;	// 拡張スタイル
	};
	class WindowResource : public WindowContextResource
	{
	public:
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		LRESULT HandleWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		HWND GetHWND();

		explicit WindowResource(WindowContext _windowContext);
		~WindowResource();

		void SetResource() override;
		
		/// <summary>
		/// リソースの初期化が完了したというフラグを trueにする
		/// </summary>
		void MarkInitialized();
		void OnResize(UINT _width, UINT _height) override;

		/// <summary>
		/// 直前のウィンドウモードに戻る
		/// </summary>
		void SetWindowMode();

		void SetFullScreen(const RECT& _monitorRect);

		/// <summary>
		/// フルスクリーンにする前に情報を保持しておく
		/// </summary>
		void GetWindowInfo();

		bool IsFullScreen() const { return isFullscreen_; }
		void SetPosition(const RECT& _monitorRect);
	private:
		void SetWindowModeImpl(WindowModeInfo _info);
		HWND hWnd_;
		bool isActive_;
		bool isFullscreen_;

		WindowModeInfo currInfo_; // 現在のウィンドウモード時の情報
		WindowModeInfo initialInfo_; // 初期のウィンドウモード時の情報


		bool isInitialized_;

		void Release() override;

		// コピーコンストラクタとコピー代入演算子を削除
		WindowResource(const WindowResource&) = delete;
		WindowResource& operator=(const WindowResource&) = delete;

	};
}
