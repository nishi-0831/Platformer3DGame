#pragma once
#include "Command.h"
#include <stack>
class CommandHistoryManager
{
public:
	/// <summary>
	/// コマンドを実行、スタックに積む
	/// </summary>
	/// <param name="_command">実行、積まれる対象のコマンド</param>
	void ExecuteCommand(Command* _command);

	/// <summary>
	/// 直前に実行したコマンドの取り消しを行う
	/// </summary>
	void UndoCommand();

	/// <summary>
	/// 直前に取り消したコマンドの再実行を行う
	/// </summary>
	void RedoCommand();

	/// <summary>
	/// <para> 全てのスタックの履歴を消去する </para>
	/// <para> コマンドは全て解放される </para>
	/// </summary>
	void ClearAllStack();
private:

	/// <summary>
	/// <para> 再実行の履歴を消去する </para>
	/// <para> コマンドは全て解放される </para>
	/// </summary>
	void ClearRedoStack();
	std::stack<Command*> undoStack_;
	std::stack<Command*> redoStack_;
};
