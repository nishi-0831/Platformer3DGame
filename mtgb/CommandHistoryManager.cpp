#include "CommandHistoryManager.h"
#include "ReleaseUtility.h"
#include "CommandHistoryManagerWrapper.h"

void CommandHistoryManager::ExecuteCommand(Command* _command)
{
	_command->Execute();
	undoStack_.push(_command);
	ClearRedoStack();
}

void CommandHistoryManager::UndoCommand()
{
	if (undoStack_.empty())
		return;
	Command* undoCommand = undoStack_.top();
	undoCommand->Undo();
	redoStack_.push(undoCommand);
	undoStack_.pop();
}

void CommandHistoryManager::RedoCommand()
{
	if (redoStack_.empty())
		return;
	Command* redoCommand = redoStack_.top();
	redoCommand->Redo();
	undoStack_.push(redoCommand);
	redoStack_.pop();
}

void CommandHistoryManager::ClearAllStack()
{
	while (undoStack_.empty())
	{
		SAFE_DELETE(undoStack_.top());
		undoStack_.pop();
	}
	ClearRedoStack();
}



void CommandHistoryManager::ClearRedoStack()
{
	while (redoStack_.empty() == false)
	{
		SAFE_DELETE(redoStack_.top());
		redoStack_.pop();
	}
}
