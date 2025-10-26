#pragma once
enum class ColliderType
{
	TYPE_SPHERE,  // ��(���S����̈�苗��)
	TYPE_CAPSULE,  // �J�v�Z��(��������̈�苗��)
	TYPE_AABB,	  // �����s���E�{�b�N�X(�e���ɕ��s�ȕ�)
};

enum class ColliderTag
{
	GAME_OBJECT,
	STAGE,
	STAGE_BOUNDARY, // �X�e�[�W�̋��E�A�͈͊O�̃R���C�_�[���Ӗ�����^�O
};