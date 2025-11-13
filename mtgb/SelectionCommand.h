#pragma once

#include "GameObjectSelectionEvent.h"
#include <functional>
#include "Command.h"

#include "Entity.h"
#include <concepts>
#include <any>
#include <map>
#include <typeindex>
namespace mtgb
{
	class SelectionCommand : public Command
	{
		using ExecuteFn = std::function<void(EntityId _entityId)>;
		using UndoFn = std::function<void(EntityId _entityId)>;
	public:
		SelectionCommand(EntityId _entityId, ExecuteFn _selectFunc, UndoFn _deselectFunc);

		void Execute() override
		{
			if (selectFunc_)
			{
				selectFunc_(entityId_);
			}
		}
		void Undo() override
		{
			if (deselectFunc_)
			{
				deselectFunc_(entityId_);
			}
		}
		std::string Name() const override { return "SelectionCommand"; }
		EntityId GetCommandTargetEntityId() const override;
	private:
		EntityId entityId_;
		ExecuteFn selectFunc_;
		UndoFn deselectFunc_;

	};

	class DeselectionCommand : public Command
	{
		using ExecuteFn = std::function<void(EntityId _entityId)>;
		using UndoFn = std::function<void(EntityId _entityId)>;
	public:
		DeselectionCommand(EntityId _entityId, ExecuteFn _deselectFunc, UndoFn _selectFunc);

		void Execute() override
		{
			if (deselectFunc_)
			{
				deselectFunc_(entityId_);
			}
		}
		void Undo() override
		{
			if (selectFunc_)
			{
				selectFunc_(entityId_);
			}
		}
		std::string Name() const override { return "DeselectionCommand"; }
		EntityId GetCommandTargetEntityId() const override;
	private:
		EntityId entityId_;
		ExecuteFn deselectFunc_;
		UndoFn selectFunc_;
	};
	
}

