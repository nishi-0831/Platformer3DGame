#pragma once
#include "Axis.h"
typedef long LONG;
namespace mtgb
{
	struct InputConfig
	{

		LONG xRange;//x���̒l�͈̔�
		LONG yRange;//y���̒l�͈̔�
		LONG zRange;//z���̒l�͈̔�

		float deadZone;//���͂𖳎�����臒l

		/// <summary>
		/// <para> x,y,z���̃A�i���O�X�e�B�b�N����擾����l�͈̔͂̐ݒ� </para>
		/// ���������A�������
		/// �ݒ肵���l��100�Ȃ�-100�`100�̒l�Ŏ���
		/// </summary>
		/// <param name="_range">�͈͂ƂȂ��Βl</param>
		void SetRange(LONG _range);

		/// <summary>
		/// <para> �w�肵�����̃A�i���O�X�e�B�b�N����擾����l�͈̔͂̐ݒ� </para>
		/// <para> ���������A������� </para>
		/// <para> �ݒ肵���l��100�Ȃ�-100�`100�̒l�Ŏ��� </para>
		/// </summary>
		/// <param name="_range">�͈͂ƂȂ��Βl</param>
		/// <param name="_axis">�ݒ肷�鎲</param>
		void SetRange(LONG _range, Axis _axis);

		/// <summary>
		/// �w�肵���l(����)���f�b�h�]�[���ɐݒ�
		/// </summary>
		/// <param name="_deadZone">0�`1�͈̔͂Ŏw�肵�Ă�������</param>
		void SetDeadZone(float _deadZone);


		/// <summary>
		/// �f�b�h�]�[����K�p����0�`1�͈̔͂ɐ��K������
		/// </summary>
		/// <param name="value">�K�p����l</param>
		/// <returns>�K�p���ꂽ�l</returns>
		float ApplyDeadZone(const float value) const;
	};
}