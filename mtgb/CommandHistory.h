#pragma once
#include "Command.h"
#include <stack>

class ICommandHistory
{
public:
	~ICommandHistory() = default;
	/// <summary>
	/// コマンドを実行、スタックに積む
	/// </summary>
	/// <param name="_command">実行、積まれる対象のコマンド</param>
	virtual void ExecuteCommand(Command* _command) = 0;

	/// <summary>
	/// 直前に実行したコマンドの取り消しを行う
	/// </summary>
	virtual void UndoCommand() = 0;

	/// <summary>
	/// 直前に取り消したコマンドの再実行を行う
	/// </summary>
	virtual void RedoCommand() = 0;

	/// <summary>
	/// <para> 全てのスタックの履歴を消去する </para>
	/// <para> コマンドは全て解放される </para>
	/// </summary>
	virtual void ClearAllStack() = 0;
protected:

	/// <summary>
	/// <para> 再実行の履歴を消去する </para>
	/// <para> コマンドは全て解放される </para>
	/// </summary>
	virtual void ClearRedoStack() = 0;
};

class CommandHistory : public ICommandHistory
{
public:
	~CommandHistory();
	/// <summary>
	/// コマンドを実行、スタックに積む
	/// </summary>
	/// <param name="_command">実行、積まれる対象のコマンド</param>
	void ExecuteCommand(Command* _command) override;

	/// <summary>
	/// 直前に実行したコマンドの取り消しを行う
	/// </summary>
	void UndoCommand() override;

	/// <summary>
	/// 直前に取り消したコマンドの再実行を行う
	/// </summary>
	void RedoCommand() override;

	/// <summary>
	/// <para> 全てのスタックの履歴を消去する </para>
	/// <para> コマンドは全て解放される </para>
	/// </summary>
	void ClearAllStack() override;
private:
	/// <summary>
	/// <para> 再実行の履歴を消去する </para>
	/// <para> コマンドは全て解放される </para>
	/// </summary>
	void ClearRedoStack();
	std::stack<Command*> undoStack_;
	std::stack<Command*> redoStack_;
};
