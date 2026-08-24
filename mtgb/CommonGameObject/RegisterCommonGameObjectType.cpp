
#include "RegisterCommonGameObjectType.h"
#include "Box3D.h"
#include "SerializableGameObject.h"
void mtgb::RegisterCommonGameObjectType(mtgb::IGameObjectRegistrar* _pGameObjectRegistrar)
{
	// ↓コピペ用
	/*
	mtgb::SpawnObjectDesc {
		.category	 = "",
		.displayName = "",
		.tooltip	 = "",
		.typeName	 = ""
	}
	*/
	_pGameObjectRegistrar->Register<mtgb::Box3D>(mtgb::SpawnObjectDesc { .category	  = "",
																		 .displayName = "Box",
																		 .tooltip	  = "ステージの地形となる箱",
																		 .typeName	  = "Box3D",
																		 .priority	  = 10 });
	_pGameObjectRegistrar->Register<mtgb::SerializableGameObject>(mtgb::SpawnObjectDesc {
		.category	 = "",
		.displayName = "EmptyObject",
		.tooltip	 = "コンポーネントを持たないオブジェクト",
		.typeName	 = "SerializableGameObject",
		.priority	 = 0 });
}