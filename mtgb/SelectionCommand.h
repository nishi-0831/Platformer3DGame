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
		using ExecuteFn = std::function<void(const GameObjectSelectedEvent&)>;
		using UndoFn = std::function<void(const GameObjectSelectedEvent&)>;
	public:
		SelectionCommand(const GameObjectSelectedEvent& _event,ExecuteFn _selectFunc, UndoFn _deselectFunc);

		void Execute() override
		{
			if (selectFunc_)
			{
				selectFunc_(event_);
			}
		}
		void Undo() override
		{
			if (deselectFunc_)
			{
				deselectFunc_(event_);
			}
		}
		std::string Name() const override { return "SelectionCommand"; }
	private:
		GameObjectSelectedEvent event_;
		ExecuteFn selectFunc_;
		UndoFn deselectFunc_;

	};

	class DeselectionCommand : public Command
	{
		using ExecuteFn = std::function<void(const GameObjectDeselectedEvent&)>;
		using UndoFn = std::function<void(const GameObjectDeselectedEvent&)>;
	public:
		DeselectionCommand(const GameObjectDeselectedEvent& _event, ExecuteFn _deselectFunc, UndoFn _selectFunc);

		void Execute() override
		{
			if (deselectFunc_)
			{
				deselectFunc_(event_);
			}
		}
		void Undo() override
		{
			if (selectFunc_)
			{
				selectFunc_(event_);
			}
		}
		std::string Name() const override { return "SelectionCommand"; }
	private:
		GameObjectDeselectedEvent event_;
		ExecuteFn deselectFunc_;
		UndoFn selectFunc_;
	};
	
}

