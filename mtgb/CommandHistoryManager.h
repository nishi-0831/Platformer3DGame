#pragma once
#include "ISystem.h"
#include "CommandHistory.h"
#include "GroupCommand.h"
namespace mtgb
{
	class CommandHistoryManager : public ISystem, public ICommandHistory
	{
	public:
		// ISystem ‚ğ‰î‚µ‚ÄŒp³‚³‚ê‚Ü‚µ‚½
		CommandHistoryManager();
		void Initialize() override;
		void Update() override;

		void BeginGroupCommand();
		void EndGroupCommand();
		// ICommandHistory ‚ğ‰î‚µ‚ÄŒp³‚³‚ê‚Ü‚µ‚½
		void ExecuteCommand(Command* _command) override;
		void UndoCommand() override;
		void RedoCommand() override;
		void ClearAllStack() override;
		void ClearRedoStack() override;
	private:
		ICommandHistory* inner_;
		bool isGrouping_;
		GroupCommand* pGroupCommand_;

	};
}