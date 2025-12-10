#include "CommandHistory.h"
#include "ReleaseUtility.h"
#include "CommandHistoryManagerWrapper.h"

CommandHistory::~CommandHistory()
{
	ClearAllStack();
}

void CommandHistory::ExecuteCommand(Command* _command)
{
	_command->Execute();
	undoStack_.push(_command);
	ClearRedoStack();
}

void CommandHistory::UndoCommand()
{
	if (undoStack_.empty())
		return;
	Command* undoCommand = undoStack_.top();
	undoCommand->Undo();
	redoStack_.push(undoCommand);
	undoStack_.pop();
}

void CommandHistory::RedoCommand()
{
	if (redoStack_.empty())
		return;
	Command* redoCommand = redoStack_.top();
	redoCommand->Redo();
	undoStack_.push(redoCommand);
	redoStack_.pop();
}

void CommandHistory::ClearAllStack()
{
	while (undoStack_.empty())
	{
		SAFE_DELETE(undoStack_.top());
		undoStack_.pop();
	}
	ClearRedoStack();
}



void CommandHistory::ClearRedoStack()
{
	while (redoStack_.empty() == false)
	{
		SAFE_DELETE(redoStack_.top());
		redoStack_.pop();
	}
}
