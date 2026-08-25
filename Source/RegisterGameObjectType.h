#pragma once
#include <Core/GameObject/GameObjectFactory.h>
#include <Core/GameObject/IGameObjectRegistrar.h>
namespace mtgb
{
	void RegisterGameObjectType(IGameObjectRegistrar* _pGameObjectRegistrar);
}