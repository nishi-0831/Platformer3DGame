#pragma once
#include "ISystem.h"
#include "cmtgb.h"

typedef union _LARGE_INTEGER LARGE_INTEGER;

namespace mtgb
{
	/// <summary>
	/// �Q�[�������Ԃ̃V�X�e���N���X
	/// </summary>
	class Time : public ISystem
	{
	public:
		Time();
		~Time();

		void Initialize() override;
		void Update() override;

		static inline const double DeltaTime() { return deltaTime_; }
		static inline const float DeltaTimeF() { return static_cast<const float>(deltaTime_); }

	private:
		static const LONGLONG SEC_TO_MICRO;  // �}�C�N���b���P�b�ɕϊ�����
		static const double MICRO_TO_SEC;  // 1�b���}�C�N���b�ɕϊ�����
		static double deltaTime_;  // �t���[���Ԏ��� (�b)

	private:
		LARGE_INTEGER current_;  // ���݂�CPU���� (�}�C�N���b)
		LARGE_INTEGER previous_;  // �O���CPU���� (�}�C�N���b)
	};
}
