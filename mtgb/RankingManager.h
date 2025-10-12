#pragma once
#include "ISystem.h"
#include "../Source/Ranking.h"

class RankingManager : public ISystem
{
public:
	RankingManager();
	~RankingManager();
	void Initialize() override;
	void Release() override;
private:
	Ranking* ranking_;
	std::vector<int> rankingList_;
	int32_t resultScore_;

	// �O�̃y�A�̃X�R�A
	int32_t prevPairScore_;
};