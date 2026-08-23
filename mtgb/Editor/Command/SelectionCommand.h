#pragma once

#include "GameObjectSelectionEvent.h"
#include <functional>
#include "Command.h"

#include <concepts>
#include <typeindex>
#include <type_traits>
#include <vector>
#include <span>
#include "CallbackConcepts.h"
#include "Core/Entity.h"
#include "SelectionMode.h"
namespace mtgb
{
	class ImGuizmoManipulator;
	class SelectionCommand : public Command
	{
	  public:
		SelectionCommand(std::span<const EntityId> _entityIds, SelectionMode _mode, ImGuizmoManipulator& _manipulator);

		void Execute() override;
		void Undo() override;
		std::string Name() const override
		{
			return "SelectionCommand";
		}

	  private:
		std::vector<EntityId> entityIds_;
		SelectionMode selectionMode_;
		ImGuizmoManipulator& manipulator_;
	};

	class DeselectionCommand : public Command
	{
	  public:
		DeselectionCommand(std::span<const EntityId> _entityIds, ImGuizmoManipulator& _manipulator);

		void Execute() override;
		void Undo() override;
		std::string Name() const override
		{
			return "DeselectionCommand";
		}

	  private:
		std::vector<EntityId> entityIds_;
		ImGuizmoManipulator& manipulator_;
	};
} // namespace mtgb
