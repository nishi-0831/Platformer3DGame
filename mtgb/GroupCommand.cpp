#include "stdafx.h"
#include "GroupCommand.h"

mtgb::GroupCommand::GroupCommand()
    : commandName_{"GroupCommand"}
    , undoCounter_{0}
    , redoCounter_{0}
{
}

mtgb::GroupCommand::~GroupCommand()
{
    
}

void mtgb::GroupCommand::Execute()
{

}

void mtgb::GroupCommand::Undo()
{
    while (undoCounter_ != 0)
    {
        commandHistoryManager_.UndoCommand();
        undoCounter_--;
        redoCounter_++;
    }
}

void mtgb::GroupCommand::Redo()
{
    while (redoCounter_ != 0)
    {
        commandHistoryManager_.RedoCommand();
        undoCounter_++;
        redoCounter_--;
    }
}

std::string mtgb::GroupCommand::Name() const
{
    return commandName_;
}

EntityId mtgb::GroupCommand::GetCommandTargetEntityId() const
{
    return INVALID_ENTITY;
}

void mtgb::GroupCommand::ExecuteCommand(Command* _pCommand)
{
    commandHistoryManager_.ExecuteCommand(_pCommand);
    undoCounter_++;
    redoCounter_ = 0;
}

void mtgb::GroupCommand::SetGroupName(std::string _name)
{
    commandName_ = _name;
}



