#pragma once
#include <string>
#include "cmtgb.h"
class Command
{
  public:
	virtual ~Command() = default;

	/// <summary>
	/// 実行
	/// </summary>
	virtual void Execute() = 0;

	/// <summary>
	/// 取り消し
	/// </summary>
	virtual void Undo() = 0;

	/// <summary>
	/// 再実行(デフォルトではExecuteと同じ)
	/// </summary>
	virtual void Redo()
	{
		Execute();
	}

	/// <summary>
	/// <para> コマンドの名前 (表示用) </para>
	/// </summary>
	/// <returns></returns>
	virtual std::string Name() const = 0;
};