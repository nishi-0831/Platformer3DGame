#pragma once
#include "cmtgb.h"

struct PlayerFellOutEvent
{
	mtgb::EntityId playerEntityId;
};

struct PlayerHpReachedZeroEvent
{
	mtgb::EntityId playerEntityId;
};