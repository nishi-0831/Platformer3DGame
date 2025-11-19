#include "stdafx.h"
#include "MovingFloorCP.h"

void MovingFloorCP::Initialize()
{
}

void MovingFloorCP::Update()
{
	for (size_t i = 0; i < poolId_.size(); i++)
	{
		if (poolId_[i] == INVALID_ENTITY) continue;

		pool_[i].Update();
	}
}
