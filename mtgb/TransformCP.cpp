#include "TransformCP.h"


mtgb::TransformCP::TransformCP()
{
}

mtgb::TransformCP::~TransformCP()
{
}

void mtgb::TransformCP::Update()
{
	for (size_t i = 0; i < poolId_.size(); i++)
	{
		if (poolId_[i] != INVALID_ENTITY)
		{
			Transform& transform = pool_[i];
			// 親がいる場合はreturn
			if (transform.HasParent()) return;
			pool_[i].Compute();

			// 親が計算をした後に、子の計算を行う
			// TODO:子のさらに子も計算する必要がある
			for (EntityId id : transform.children_)
			{
				pool_[id].Compute();
			}
		}
	}
}
