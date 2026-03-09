#pragma once

#include "GameObjectSelectionEvent.h"
#include <functional>
#include "Command.h"

#include <concepts>
#include <typeindex>
#include <type_traits>
#include "CallbackConcepts.h"
#include "Entity.h"
namespace mtgb
{
	class SelectionCommand : public Command
	{
	  public:
		template <EntityCallable FuncA, EntityCallable FuncB>
		SelectionCommand(EntityId _entityId, FuncA&& _selectFunc, FuncB&& _deselectFunc);

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
		std::string Name() const override
		{
			return "SelectionCommand";
		}
		EntityId GetCommandTargetEntityId() const override;

	  private:
		EntityId entityId_;
		EntityCallback selectFunc_;
		EntityCallback deselectFunc_;
	};

	class DeselectionCommand : public Command
	{
	  public:
		template <EntityCallable FuncA, EntityCallable FuncB>
		DeselectionCommand(EntityId _entityId, FuncA&& _deselectFunc, FuncB&& _selectFunc);

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
		std::string Name() const override
		{
			return "DeselectionCommand";
		}
		EntityId GetCommandTargetEntityId() const override;

	  private:
		EntityId entityId_;
		EntityCallback deselectFunc_;
		EntityCallback selectFunc_;
	};

	template <EntityCallable FuncA, EntityCallable FuncB>
	inline SelectionCommand::SelectionCommand(EntityId _entityId, FuncA&& _selectFunc, FuncB&& _deselectFunc)
		: entityId_ { _entityId }
		, selectFunc_ { std::forward<FuncA>(_selectFunc) }
		, deselectFunc_ { std::forward<FuncB>(_deselectFunc) }
	{
	}

	template <EntityCallable FuncA, EntityCallable FuncB>
	inline DeselectionCommand::DeselectionCommand(EntityId _entityId, FuncA&& _deselectFunc, FuncB&& _selectFunc)
		: entityId_ { _entityId }
		, deselectFunc_ { std::forward<FuncA>(_deselectFunc) }
		, selectFunc_ { std::forward<FuncB>(_selectFunc) }
	{
	}

} // namespace mtgb
