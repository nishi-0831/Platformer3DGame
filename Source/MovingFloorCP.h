#pragma once
#include "ComponentPool.h"
#include "MovingFloor.h"
class MovingFloor;
class MovingFloorCP : public ComponentPool<MovingFloor, MovingFloorCP>
{
public:
	void Initialize();
	void Update() override;
};