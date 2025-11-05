#include "stdafx.h"
#include "RegisterGameObjectType.h"
#include "Player.h"
void mtgb::RegisterGameObjectType(mtgb::IGameObjectRegistrar* _pGameObjectRegistrar)
{
	_pGameObjectRegistrar->Register<Player>("Player");
}
