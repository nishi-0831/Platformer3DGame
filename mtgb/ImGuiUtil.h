#pragma once
#include <optional>
#include "../ImGui/imgui.h"

#include "Vector3.h"
#include "Matrix4x4.h"
struct D3D11_VIEWPORT;
namespace mtgb
{
	namespace ImGuiUtil
	{
		void GetMouseRay(Vector3& _near, Vector3& _far, const Matrix4x4& _proj, const Matrix4x4& _view, const D3D11_VIEWPORT& _viewport);
		bool IsMouseInWindow(const char* _name);
		std::optional<ImVec2> WorldToImGui(const Vector3& _vec, const Matrix4x4& _proj, const Matrix4x4& _view, const D3D11_VIEWPORT& _viewport);
	}
}