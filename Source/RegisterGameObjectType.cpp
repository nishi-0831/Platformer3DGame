#include "stdafx.h"
#include "RegisterGameObjectType.h"
#include "Player.h"
#include "ScoreItem.h"
#include "Goal.h"
#include "GameOverZone.h"
#include "MovingFloor.h"
#include "PatrolChargerEnemy.h"
void mtgb::RegisterGameObjectType(mtgb::IGameObjectRegistrar* _pGameObjectRegistrar)
{
	_pGameObjectRegistrar->Register<MovingFloor>("MovingFloor");
	_pGameObjectRegistrar->Register<Player>("Player");
	_pGameObjectRegistrar->Register<ScoreItem>("ScoreItem");
	_pGameObjectRegistrar->Register<Goal>("Goal");
	_pGameObjectRegistrar->Register<GameOverZone>("GameOverZone");
	_pGameObjectRegistrar->Register<PatrolChargerEnemy>("PatrolChargerEnemy");
}
