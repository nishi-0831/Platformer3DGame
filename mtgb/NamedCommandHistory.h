#pragma once
#include "CommandHistoryManager.h"
#include <vector>
#include <deque>
#include <string>

namespace mtgb
{
	class NamedCommandHistory : public ICommandHistory
	{
	  public:
		explicit NamedCommandHistory(ICommandHistory* _inner = nullptr);
		~NamedCommandHistory();

		void ExecuteCommand(Command* _pCommand) override;
		void UndoCommand() override;
		void RedoCommand() override;
		void ClearAllStack() override;

		const std::deque<std::string>& GetUndoNames() const
		{
			return undoNames_;
		};
		const std::deque<std::string>& GetRedoNames() const
		{
			return redoNames_;
		};

		void DrawImGuiStack() const;

	  private:
		void ClearRedoStack() override;
		ICommandHistory* inner_ = nullptr;
		std::deque<std::string> undoNames_;
		std::deque<std::string> redoNames_;
		std::deque<std::string> commandHistoryNames_;
	};
} // namespace mtgb