
#include "RegisterCommonGameObjectType.h"
#include "Box3D.h"
#include "SerializableGameObject.h"
void mtgb::RegisterCommonGameObjectType(mtgb::IGameObjectRegistrar* _pGameObjectRegistrar)
{
	_pGameObjectRegistrar->Register<mtgb::Box3D>("Box3D");
	_pGameObjectRegistrar->Register<mtgb::SerializableGameObject>("SerializableGameObject");
}