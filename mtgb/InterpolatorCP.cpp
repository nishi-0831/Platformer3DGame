#include "stdafx.h"
#include "InterpolatorCP.h"
#include "Interpolator.h"
mtgb::InterpolatorCP::InterpolatorCP()
{
}

mtgb::InterpolatorCP::~InterpolatorCP()
{
}

void mtgb::InterpolatorCP::Update()
{
	for (size_t i = 0; i < poolId_.size(); i++)
	{
		if (poolId_[i] == INVALID_ENTITY) continue;

		//pool_[i].UpdateTransform();
	}
}
