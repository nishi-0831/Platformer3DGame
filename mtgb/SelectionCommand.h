#pragma once

#include <functional>
#include "Command.h"

#include "Entity.h"
#include <concepts>
#include <any>
#include <map>
#include <typeindex>
namespace mtgb
{
	/*class SelectionCommand : public Command
	{
		using ExecuteFn = std::function<void(EntityId)>;
		using UndoFn = std::function<void()>;
	public:
		SelectionCommand(EntityId _newId,EntityId _oldId,std::function<void(EntityId)> _selectFunc, std::function<void(EntityId)> _deselectFunc);

		void Execute() override
		{
			if (selectFunc_)
			{
				selectFunc_(newId_);
			}
			else
			{
			}
		}
		std::string Name() const override { return "SelectionCommand"; }
	private:
		EntityId newId_;
		EntityId oldId_;
		std::function<void(EntityId)> selectFunc_;
		std::function<void()> deselectFunc_;

	};*/

	
}

