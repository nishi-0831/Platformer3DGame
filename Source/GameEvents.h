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

struct PlayerReachedGoalEvent
{
};

struct PlayerCollideRespawnPointEvent
{
	mtgb::EntityId playerEntityId;
	mtgb::EntityId respawnPointEntityId;
	int priority; // リスポーンポイントの優先度。例えば、ゴールに近いものを優先したい場合などに使う
};