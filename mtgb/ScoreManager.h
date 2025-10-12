#pragma once
#include "ISystem.h"
#include <cstdint>

namespace mtgb
{
	class ScoreManager : public ISystem
	{
	public:
		ScoreManager();
		~ScoreManager();
		void Initialize() override {}
		void Update() override;

		/// <summary>
		/// �X�R�A�̉��_���s��
		/// </summary>
		static void AddScore(uint32_t _score);
		/// <summary>
		/// �m���}�̉��_���s��
		/// </summary>
		static void AddQuota(uint32_t _quota) { ScoreManager::quotaScore_ += _quota; }

		/// <summary>
		/// ���݂̃X�R�A���擾����
		/// </summary>
		static int32_t GetScore();

		/// <summary>
		/// �X�R�A�̌��_���s��
		/// </summary>
		/// <param name="_score"></param>
		static void SubtractScore(uint32_t _score);
		/// <summary>
		/// �m���}�̃X�R�A���擾����
		/// </summary>
		/// <returns></returns>
		static int32_t GetQuotaScore();
		/// <summary>
		/// �X�R�A�̍ő�l���擾����
		/// </summary>
		/// <returns>�X�R�A�l</returns>
		static int32_t GetQuotaScoreMax();
		/// <summary>
		/// �X�R�A�����Z�b�g����
		/// </summary>
		static void ResetScore();

		/// <summary>
		/// �m���}��B��������
		/// </summary>
		/// <returns>�B�����Ă���Ȃ�^��Ԃ�</returns>
		static bool AchievedQuota();
	private:
		static int32_t score_;
		// �m���}�̃X�R�A
		static int32_t quotaScore_;
	};
}