#include "stdafx.h"
#include "RegisterGameObjectType.h"
#include "Player.h"
#include "ScoreItem.h"
void mtgb::RegisterGameObjectType(mtgb::IGameObjectRegistrar* _pGameObjectRegistrar)
{
	_pGameObjectRegistrar->Register<Player>("Player");
	_pGameObjectRegistrar->Register<ScoreItem>("ScoreItem");
}
