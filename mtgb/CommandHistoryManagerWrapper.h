#pragma once
#include "CommandHistoryManager.h"
#include <vector>
#include <deque>
#include <string>

namespace mtgb
{
	class CommandHistoryManagerWrapper
	{
	public:
		// ˆÃ–Ù“I‚È•ÏŠ·‚ð‹ÖŽ~
		explicit CommandHistoryManagerWrapper(CommandHistoryManager* _inner = nullptr);
		~CommandHistoryManagerWrapper() = default;

		void ExecuteCommand(Command* _command);
		void UndoCommand();
		void RedoCommand();
		void ClearAllStack();

		const std::deque<std::string>& GetUndoNames() const { return undoNames_; };
		const std::deque<std::string>& GetRedoNames() const { return redoNames_; };

		void DrawImGuiStack() const;
	private:
		CommandHistoryManager* inner_ = nullptr;
		std::deque<std::string> undoNames_;
		std::deque<std::string> redoNames_;
		std::deque<std::string> commandHistoryNames_;
	};
}