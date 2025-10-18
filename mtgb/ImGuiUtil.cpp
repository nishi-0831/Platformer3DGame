#include "ImGuiUtil.h"
#include "../ImGui/imgui_internal.h"
#include <d3d11.h>

void mtgb::ImGuiUtil::GetMouseRay(Vector3& _near, Vector3& _far, const Matrix4x4& _proj, const Matrix4x4& _view, const D3D11_VIEWPORT& _viewport, const Vector2F& _windowPos)
{
	ImVec2 mousePos = ImGui::GetMousePos();
    
	ImVec2 localPos = ImVec2(mousePos.x - _windowPos.x, mousePos.y - _windowPos.y);

	Vector3 nearVec = { localPos.x,localPos.y,0.0f };

	_near = DirectX::XMVector3Unproject(
		nearVec,
		_viewport.TopLeftX,
		_viewport.TopLeftY,
		_viewport.Width,
		_viewport.Height,
		_viewport.MinDepth,
		_viewport.MaxDepth,
		_proj,
		_view,
		DirectX::XMMatrixIdentity());

	Vector3 farVec = { static_cast<float>(localPos.x),static_cast<float>(localPos.y),1.0f };
	_far = DirectX::XMVector3Unproject(
		farVec,
		_viewport.TopLeftX,
		_viewport.TopLeftY,
		_viewport.Width,
		_viewport.Height,
		_viewport.MinDepth,
		_viewport.MaxDepth,
		_proj,
		_view,
		DirectX::XMMatrixIdentity());
}



bool mtgb::ImGuiUtil::IsMouseInWindow(const char* _name)
{
	//	ウィンドウを検索
	ImGuiWindow* window = ImGui::FindWindowByName(_name);
	if (window && window->WasActive)
	{
		ImVec2 mousePos = ImGui::GetIO().MousePos;
		ImRect rect = window->WorkRect;
		return rect.Contains(mousePos);
	}
	return false;
}

std::optional<ImVec2> mtgb::ImGuiUtil::WorldToImGui(const Vector3& _vec, const Matrix4x4& _proj, const Matrix4x4& _view, const D3D11_VIEWPORT& _viewport)
{
	float tabBarHeight = ImGui::GetCurrentWindow()->TitleBarHeight;
	ImVec2 windowPos = ImGui::GetWindowPos();
	mtgb::Vector3 screenVec = DirectX::XMVector3Project(
		_vec,
		_viewport.TopLeftX,
		_viewport.TopLeftY,
		_viewport.Width,
		_viewport.Height,
		_viewport.MinDepth,
		_viewport.MaxDepth,
		_proj,
		_view,
		DirectX::XMMatrixIdentity());

	if (screenVec.z < 0.0f || screenVec.z > 1.0f)
		return std::nullopt;
	//return ImVec2(screenVec.x, screenVec.y );
	return ImVec2(screenVec.x + windowPos.x, screenVec.y + windowPos.y + tabBarHeight);
}