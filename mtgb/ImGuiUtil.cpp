#include "ImGuiUtil.h"
#include "ImGui/imgui_internal.h"
#include <d3d11.h>

void mtgb::ImGuiUtil::GetMouseRay(
	Vector3& _near,
	Vector3& _far,
	const Matrix4x4& _proj,
	const Matrix4x4& _view,
	const D3D11_VIEWPORT& _viewport,
	const Vector2F& _windowPos
)
{
	ImVec2 mousePos = ImGui::GetMousePos();

	ImVec2 localPos = ImVec2(mousePos.x - _windowPos.x, mousePos.y - _windowPos.y);

	Vector3 nearVec = { localPos.x, localPos.y, 0.0f };

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
		DirectX::XMMatrixIdentity()
	);

	Vector3 farVec = { static_cast<float>(localPos.x), static_cast<float>(localPos.y), 1.0f };

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
		DirectX::XMMatrixIdentity()
	);
}

bool mtgb::ImGuiUtil::IsMouseInWindow(const char* _name)
{
	//	ウィンドウを検索
	ImGuiWindow* window = ImGui::FindWindowByName(_name);
	if (window && window->WasActive)
	{
		ImVec2 mousePos = ImGui::GetIO().MousePos;
		ImRect rect		= window->WorkRect;
		return rect.Contains(mousePos);
	}
	return false;
}

std::optional<ImVec2> mtgb::ImGuiUtil::WorldToImGui(
	const Vector3& _vec,
	const Matrix4x4& _proj,
	const Matrix4x4& _view,
	const D3D11_VIEWPORT& _viewport
)
{
	float tabBarHeight		= ImGui::GetCurrentWindow()->TitleBarHeight;
	ImVec2 windowPos		= ImGui::GetWindowPos();
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
		DirectX::XMMatrixIdentity()
	);

	if (screenVec.z < 0.0f || screenVec.z > 1.0f)
		return std::nullopt;
	// return ImVec2(screenVec.x, screenVec.y );
	return ImVec2(screenVec.x + windowPos.x, screenVec.y + windowPos.y + tabBarHeight);
}

std::optional<std::pair<ImVec2, ImVec2>> mtgb::ImGuiUtil::WorldToImGuiClipped(
	const Vector3& _from,
	const Vector3& _to,
	const Matrix4x4& _proj,
	const Matrix4x4& _view,
	const D3D11_VIEWPORT& _viewport,
	float _near,
	float _far
)
{
	using namespace DirectX;
	// 二つの頂点をビュー空間に変換
	DirectX::XMVECTOR viewFrom = DirectX::XMVector3Transform(_from, _view);
	DirectX::XMVECTOR viewTo   = DirectX::XMVector3Transform(_to, _view);
	// Z座標を取得
	float fromZ = XMVectorGetZ(viewFrom);
	float toZ	= XMVectorGetZ(viewTo);
	// 両方ともカメラの後ろなら描画しない
	if (fromZ < _near && toZ < _near)
		return std::nullopt;

	// カメラの後ろにある点を、ニアクリップ面にクリッピングする

	// 補間係数tを、 手前の点からニアクリップ面までの距離 ÷ 奥の点から手前の点までの距離で求める
	// lerpによって、手前にある点の座標をニアクリップ面上の交点にする
	if (fromZ < _near)
	{
		float t	 = (_near - fromZ) / (toZ - fromZ);
		viewFrom = XMVectorLerp(viewFrom, viewTo, t);
	}
	if (toZ < _near)
	{
		float t = (_near - toZ) / (fromZ - toZ);
		viewTo	= XMVectorLerp(viewTo, viewFrom, t);
	}

	// クリッピングした「ビュー空間の」座標をワールド座標に戻す
	// XMVector3Projectはワールド座標を入力として受け取るから
	XMMATRIX invView		   = XMMatrixInverse(nullptr, _view);
	XMVECTOR finalFromWorldPos = XMVector3Transform(viewFrom, invView);
	XMVECTOR finalToWorldPos   = XMVector3Transform(viewTo, invView);

	// 両端点をスクリーン座標に変換
	auto screenFrom = DirectX::XMVector3Project(
		finalFromWorldPos,
		_viewport.TopLeftX,
		_viewport.TopLeftY,
		_viewport.Width,
		_viewport.Height,
		_viewport.MinDepth,
		_viewport.MaxDepth,
		_proj,
		_view,
		DirectX::XMMatrixIdentity()
	);

	auto screenTo = DirectX::XMVector3Project(
		finalToWorldPos,
		_viewport.TopLeftX,
		_viewport.TopLeftY,
		_viewport.Width,
		_viewport.Height,
		_viewport.MinDepth,
		_viewport.MaxDepth,
		_proj,
		_view,
		DirectX::XMMatrixIdentity()
	);

	// ImGuiウィンドウの座標、タブバーの高さ
	float tabBarHeight = ImGui::GetCurrentWindow()->TitleBarHeight;
	ImVec2 windowPos   = ImGui::GetWindowPos();

	ImVec2 p1(
		DirectX::XMVectorGetX(screenFrom) + windowPos.x,
		DirectX::XMVectorGetY(screenFrom) + windowPos.y + tabBarHeight
	);
	ImVec2 p2(
		DirectX::XMVectorGetX(screenTo) + windowPos.x,
		DirectX::XMVectorGetY(screenTo) + windowPos.y + tabBarHeight
	);

	return std::make_pair(p1, p2);
}
