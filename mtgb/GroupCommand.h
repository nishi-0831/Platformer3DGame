#pragma once
#include "CommandHistory.h"
#include <string>
namespace mtgb
{
	
	class GroupCommand : public Command 
	{
	public:
		GroupCommand();
		~GroupCommand();
		void Execute() override;
		void Undo() override;
		void Redo() override;
		std::string Name() const override;
		EntityId GetCommandTargetEntityId() const override;
		void ExecuteCommand(Command* _pCommand);
		void SetGroupName(std::string _name);
	private:
		std::string commandName_;
		unsigned int undoCounter_;
		unsigned int redoCounter_;
		CommandHistory commandHistoryManager_;
	};

}