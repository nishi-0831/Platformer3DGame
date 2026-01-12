#include "GameObjectLayer.h"

namespace
{
	mtgb::GameObjectLayerFlag all{mtgb::GameObjectLayerFlag().BeginEdit().OnAll().EndEdit()};
}
mtgb::GameObjectLayerFlag mtgb::AllLayer()
{
	return all;
}
