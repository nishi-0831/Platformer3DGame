#pragma once
#include <dxgi.h>

namespace mtgb
{
	struct MonitorInfo
	{
	int adapterIndex; // ���蓖�Ă��Ă���A�_�v�^�[(GPU)�̃C���f�b�N�X
	int outputIndex; // ���蓖�Ă��Ă��郂�j�^�[�̃C���f�b�N�X
	bool isRequested; // �g�p�ς݂��ǂ����̃t���O
	DXGI_OUTPUT_DESC desc;
	};
}
