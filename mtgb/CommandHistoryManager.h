#pragma once
#include "ISystem.h"
#include "CommandHistory.h"
#include "GroupCommand.h"
namespace mtgb
{
	class CommandHistoryManager : public ISystem, public ICommandHistory
	{
	  public:
		// ISystem を介して継承されました
		CommandHistoryManager();
		void Initialize() override;
		void Update() override;

		void BeginGroupCommand();
		void EndGroupCommand();
		// ICommandHistory を介して継承されました
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
} // namespace mtgb