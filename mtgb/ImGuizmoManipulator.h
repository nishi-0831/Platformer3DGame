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
#include "Transform.h"
#include "ComponentFactory.h"
namespace mtgb
{
	class GameObject;
	struct Vector3;
	class ImGuizmoManipulator : public ImGuiShowable
	{
	public:
		ImGuizmoManipulator(std::function<void(Command*)> _commandListener, const ComponentFactory& _componentFactory);
		~ImGuizmoManipulator();

		void Initialize();
		void Update();
		void ShowImGui() override;
		
		void Select(EntityId _id);
		void Deselect();
		EntityId GetSelectedEntityId();
	private:
		void GenerateCommand(const GameObjectSelectedEvent& _event);
		void GenerateCommand(const GameObjectDeselectedEvent& _event);
		void SubscribeGameObjectSelectionEvent();

		void DrawTransformGuizmo();
		void Calculate();

		std::function<void(Command*)> commandListener_;

		// ComponentFactory‚ªnull‚Å‚ ‚é‚±‚Æ‚ð‹–—e‚µ‚È‚¢
		const ComponentFactory& componentFactory_;
		std::size_t hGameObjSelectionEvent;

		Transform* pTargetTransform_;
		ImGuizmo::OPERATION operation_;
		ImGuizmo::MODE mode_;
		Transform* pCameraTransform_;
		uintptr_t currId_;
	
		float worldMat_[16], viewMat_[16], projMat_[16];
		Matrix4x4 worldMatrix4x4, viewMatrix4x4_, projMatrix4x4_;
		DirectX::XMFLOAT4X4 float4x4_;


		bool wasUsing_;
		bool isUsing_;
		TransformMemento* pTargetPrevTransformMemento_;
		TransformMemento* pTargetTransformMemento_;

	};
}
