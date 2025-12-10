#include "CommandHistoryManagerWrapper.h"
#include "../ImGui/imgui.h"

namespace
{
	const size_t HISTORY_CAPACITY = 256;
}

namespace mtgb
{
	NamedCommandHistory::NamedCommandHistory(ICommandHistory* _inner)
		: inner_{_inner}
	{

	}

	void NamedCommandHistory::ExecuteCommand(Command* _command)
	{
		if (inner_ == nullptr) return;
		if (_command == nullptr) return;
		inner_->ExecuteCommand(_command);

		commandHistoryNames_.push_back("Execute:" + _command->Name());
		undoNames_.push_back(_command->Name());
		redoNames_.clear();

		// サイズの制限
		if (commandHistoryNames_.size() > HISTORY_CAPACITY)
		{
			commandHistoryNames_.pop_front();
		}
		if (undoNames_.size() > HISTORY_CAPACITY)
		{
			undoNames_.pop_front();
		}
	}

	void NamedCommandHistory::UndoCommand()
	{
		if (inner_ == nullptr) return;
		inner_->UndoCommand();

		if (undoNames_.empty()) return;
		commandHistoryNames_.push_back("Undo:" + undoNames_.back());
		// undo -> redo へ名前を移動
		redoNames_.push_back(undoNames_.back());
		undoNames_.pop_back();

		// サイズの制限
		if (commandHistoryNames_.size() > HISTORY_CAPACITY)
		{
			commandHistoryNames_.pop_front();
		}
		if (redoNames_.size() > HISTORY_CAPACITY)
		{
			redoNames_.pop_front();
		}
	}

	void NamedCommandHistory::RedoCommand()
	{
		if (inner_ == nullptr) return;
		inner_->RedoCommand();

		if (redoNames_.empty()) return;
		commandHistoryNames_.push_back("Redo:" + redoNames_.back());
		undoNames_.push_back(redoNames_.back());
		redoNames_.pop_back();

		// サイズの制限
		if (commandHistoryNames_.size() > HISTORY_CAPACITY)
		{
			commandHistoryNames_.pop_front();
		}
		if (undoNames_.size() > HISTORY_CAPACITY)
		{
			undoNames_.pop_front();
		}
	}

	void NamedCommandHistory::ClearAllStack()
	{
		if (inner_ == nullptr) return;
		inner_->ClearAllStack();
		commandHistoryNames_.clear();
		undoNames_.clear();
		redoNames_.clear();
	}

	void NamedCommandHistory::DrawImGuiStack() const
	{
		ImGui::BeginChild("CommandHistory", ImVec2(0, 0));

		ImGui::Text("&s", "CommandHisotory");
		ImGui::Separator();

		ImGui::Text("Undo (%zu)", undoNames_.size());
		for (int i = static_cast<int>( undoNames_.size() - 1); i >= 0; --i)
		{
			ImGui::BulletText("%s", undoNames_[i].c_str());
		}

		ImGui::Spacing();
		ImGui::Text("Redo (%zu):", redoNames_.size());
		for (int i = static_cast<int>(redoNames_.size() - 1); i >= 0; --i)
		{
			ImGui::BulletText("%s", redoNames_[i].c_str());
		}

		ImGui::Spacing();
		ImGui::Text("Command History (%zu)", commandHistoryNames_.size());
		for (int i = static_cast<int>(commandHistoryNames_.size() - 1); i >= 0; --i)
		{
			ImGui::BulletText("%s", commandHistoryNames_[i].c_str());
		}

		ImGui::EndChild();
	}
	void NamedCommandHistory::ClearRedoStack()
	{		
		redoNames_.clear();
	}
}
