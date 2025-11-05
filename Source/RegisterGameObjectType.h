#pragma once
#include "../mtgb/GameObjectFactory.h"
#include "../mtgb/IGameObjectRegistrar.h"
namespace mtgb
{
	void RegisterGameObjectType(IGameObjectRegistrar* _pGameObjectRegistrar);
}