
#include "RegisterGameObjectType.h"
#include "Box3D.h"
void mtgb::RegisterGameObjectType(GameObjectFactory* _pGameObjFactory)
{
	_pGameObjFactory->Register<mtgb::Box3D>("Box3D");
}
