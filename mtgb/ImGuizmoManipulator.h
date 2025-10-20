#pragma once
#include "ImGuiShowable.h"
#include "Matrix4x4.h"

#include "../ImGui/imgui_impl_win32.h"
#include "../ImGui/ImGuizmo.h"
#include <functional>
#include "Command.h"
#include "CommandHandlerRegistry.h"
#include "GameObjectSelectionEvent.h"
#include "SelectionCommand.h"
namespace mtgb
{
	class GameObject;
	class Transform;
	struct Vector3;

	class ImGuizmoManipulator : public ImGuiShowable
	{
	public:
		ImGuizmoManipulator(std::function<void(Command*)> _commandListener);
		~ImGuizmoManipulator();

		void Initialize();
		void Update();
		void ShowImGui() override;
		
		void Select(EntityId _id);
		void Deselect();
	private:
		void GenerateCommand(const GameObjectSelectedEvent& _event);
		void GenerateCommand(const GameObjectDeselectedEvent& _event);
		std::function<void(Command*)> commandListener_;
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
	};
}
