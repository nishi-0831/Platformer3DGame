#include "GameObjectLayer.h"

namespace
{
    mtgb::GameObjectLayerFlag all{ mtgb::GameObjectLayerFlag::BitFlag().BeginEdit().OnAll().EndEdit() };
}
mtgb::GameObjectLayerFlag mtgb::AllLayer()
{
    return all;
}
