#pragma once
#include "Command.h"
#include <stack>
class CommandHistoryManager
{
public:
	/// <summary>
	/// �R�}���h�����s�A�X�^�b�N�ɐς�
	/// </summary>
	/// <param name="_command">���s�A�ς܂��Ώۂ̃R�}���h</param>
	void ExecuteCommand(Command* _command);

	/// <summary>
	/// ���O�Ɏ��s�����R�}���h�̎��������s��
	/// </summary>
	void UndoCommand();

	/// <summary>
	/// ���O�Ɏ��������R�}���h�̍Ď��s���s��
	/// </summary>
	void RedoCommand();

	/// <summary>
	/// <para> �S�ẴX�^�b�N�̗������������� </para>
	/// <para> �R�}���h�͑S�ĉ������� </para>
	/// </summary>
	void ClearAllStack();
private:

	/// <summary>
	/// <para> �Ď��s�̗������������� </para>
	/// <para> �R�}���h�͑S�ĉ������� </para>
	/// </summary>
	void ClearRedoStack();
	std::stack<Command*> undoStack_;
	std::stack<Command*> redoStack_;
};
