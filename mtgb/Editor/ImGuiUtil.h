#pragma once
#include <optional>
#include "ImGui/imgui.h"

#include "Math/Vector3.h"
#include "Math/Matrix4x4.h"
#include "Math/Vector2F.h"
struct D3D11_VIEWPORT;
namespace mtgb
{
	namespace ImGuiUtil
	{
		/// <summary>
		/// シーンビューの空間に変換したレイを作成する
		/// </summary>
		/// <param name="_start">始点</param>
		/// <param name="_end">終点</param>
		/// <param name="_proj">カメラの射影行列</param>
		/// <param name="_view">カメラのビュー行列</param>
		/// <param name="_viewport">シーンビューのビューポート</param>
		/// <param name="_windowPos">シーンビューの座標</param>
		void GetMouseRay(
			Vector3& _start,
			Vector3& _end,
			const Matrix4x4& _proj,
			const Matrix4x4& _view,
			const D3D11_VIEWPORT& _viewport,
			const Vector2F& _windowPos
		);
		/// <summary>
		/// マウスが、指定した名前のウィンドウの中にあるか否か
		/// </summary>
		/// <param name="_name">ウィンドウ名</param>
		/// <returns></returns>
		bool IsMouseInWindow(const char* _name);
		/// <summary>
		/// ワールド座標をシーンビューのスクリーン座標に変換する
		/// </summary>
		/// <param name="_vec"></param>
		/// <param name="_proj"></param>
		/// <param name="_view"></param>
		/// <param name="_viewport"></param>
		/// <returns></returns>
		std::optional<ImVec2> WorldToImGui(
			const Vector3& _vec,
			const Matrix4x4& _proj,
			const Matrix4x4& _view,
			const D3D11_VIEWPORT& _viewport
		);
		/// <summary>
		/// ワールド座標系の線分をシーンビューのスクリーン座標に変換する。
		/// シーンビューのウィンドウにクリップした状態で返す
		/// </summary>
		/// <param name="_from"></param>
		/// <param name="_to"></param>
		/// <param name="_proj"></param>
		/// <param name="_view"></param>
		/// <param name="_viewport"></param>
		/// <param name="_near"></param>
		/// <param name="_far"></param>
		/// <returns></returns>
		std::optional<std::pair<ImVec2, ImVec2>> WorldToImGuiClipped(
			const Vector3& _from,
			const Vector3& _to,
			const Matrix4x4& _proj,
			const Matrix4x4& _view,
			const D3D11_VIEWPORT& _viewport,
			float _near,
			float _far
		);
	} // namespace ImGuiUtil
} // namespace mtgb