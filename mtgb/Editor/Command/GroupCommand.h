#pragma once
#include "CommandHistory.h"
#include <string>
namespace mtgb
{
	/// <summary>
	/// 複数のコマンドをひとまとめするコマンド
	/// </summary>
	class GroupCommand : public Command
	{
	  public:
		GroupCommand();
		~GroupCommand();
		void Execute() override;
		void Undo() override;
		void Redo() override;
		std::string Name() const override;
		void ExecuteCommand(Command* _pCommand);
		/// <summary>
		/// ひとまとめにされたコマンドの名前を設定する。
		/// </summary>
		/// <param name="_name"></param>
		void SetGroupName(std::string_view _name);

	  private:
		std::string commandName_;
		unsigned int undoCounter_;
		unsigned int redoCounter_;
		CommandHistory commandHistory_;
	};

} // namespace mtgb