#pragma once
#include <string>

class Command
{
public:
	virtual ~Command() = default;

	/// <summary>
	/// ���s
	/// </summary>
	virtual void Execute() = 0;
	
	/// <summary>
	/// ������
	/// </summary>
	virtual void Undo() = 0;

	/// <summary>
	/// �Ď��s(Execute�Ɠ���)
	/// </summary>
	virtual void Redo() { Execute(); }

	/// <summary>
	/// <para> �R�}���h�̖��O (�\���p) </para>
	/// <para> �R�s�[�ɂ�镉�ׂ��傫���ꍇ�� string_view�Ȃǂ֐؂�ւ��邱�Ƃ�����ɓ���� </para>
	/// </summary>
	/// <returns></returns>
	virtual std::string Name() const = 0;
};