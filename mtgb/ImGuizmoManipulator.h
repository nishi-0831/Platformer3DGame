#pragma once
#include "ImGuiShowable.h"
#include "Matrix4x4.h"
#include "../ImGui/imgui_impl_win32.h"
#include "../ImGui/ImGuizmo.h"
#include <d3d11.h>

namespace mtgb
{

	class GameObject;
	class Transform;
	struct Vector3;

	class ImGuizmoManipulator : public ImGuiShowable
	{
		friend class ImGuiRenderer;
	public:
		ImGuizmoManipulator();
		~ImGuizmoManipulator();

		void SetCamera();
		void ShowImGui() override;
		
		//std::optional<ImVec2> WorldToImGui(const Vector3& _vec);
		void SetTarget(Transform* _pTarget);
		void SetViewport(const D3D11_VIEWPORT& _viewport);
	private:
		void SubscribeGameObjectSelectionEvent();

		void DrawTransformGuizmo();
		void Calculate();
		std::size_t hGameObjSelectionEvent;

		Transform* pTargetTransform_;
		ImGuizmo::OPERATION operation_;
		ImGuizmo::MODE mode_;
		Transform* pCameraTransform_;
		uintptr_t currId_;
	
		float worldMat_[16], viewMat_[16], projMat_[16];
		Matrix4x4 worldMatrix4x4, viewMatrix4x4_, projMatrix4x4_;
		DirectX::XMFLOAT4X4 float4x4_;
		D3D11_VIEWPORT viewport_;
	};
}
