
#include "RegisterCommonGameObjectType.h"
#include "Box3D.h"
void mtgb::RegisterCommonGameObjectType(mtgb::IGameObjectRegistrar* _pGameObjectRegistrar)
{
	_pGameObjectRegistrar->Register<mtgb::Box3D>("Box3D");
}