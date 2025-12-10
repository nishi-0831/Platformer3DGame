#include "stdafx.h"
#include "CommandHistoryManager.h"
#include "assert.h"
CommandHistoryManager::CommandHistoryManager()
	: inner_{ new CommandHistory() }
	, pGroupCommand_{ nullptr }
	, isGrouping_{false}
{
}

void CommandHistoryManager::Initialize()
{
}

void CommandHistoryManager::Update()
{
}

void CommandHistoryManager::BeginGroupCommand()
{
	if (isGrouping_)
		return;

	isGrouping_ = true;
	SAFE_DELETE(pGroupCommand_);
	pGroupCommand_ = new GroupCommand();
}

void CommandHistoryManager::EndGroupCommand()
{
	if (isGrouping_ == false)
		return;
	inner_->ExecuteCommand(pGroupCommand_);
}



void CommandHistoryManager::ExecuteCommand(Command* _command)
{
	if (isGrouping_ && pGroupCommand_ != nullptr)
	{
		pGroupCommand_->ExecuteCommand(_command);
	}
}

void CommandHistoryManager::UndoCommand()
{
	if (isGrouping_)
	{
		assert(false && " コマンドをまとめている最中のUndoは許可されていません。 CancellGroupCommand() を呼んでください ");
	}

	inner_->UndoCommand();
}

void CommandHistoryManager::RedoCommand()
{
	if (isGrouping_)
	{
		assert(false && " コマンドをまとめている最中のRedoは許可されていません。 CancellGroupCommand() を呼んでください ");
		return;
	}

	inner_->RedoCommand();
}

void CommandHistoryManager::ClearAllStack()
{
	SAFE_DELETE(pGroupCommand_);
	inner_->ClearAllStack();
}

void CommandHistoryManager::ClearRedoStack()
{
	
}
