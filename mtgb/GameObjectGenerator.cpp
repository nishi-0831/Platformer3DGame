#include "GameObjectGenerator.h"
#include "GameObject.h"
#include "Box3D.h"


mtgb::EntityId mtgb::GameObjectGenerator::GeneratePrimitive(PrimitiveType _primitive)
{
    return  GameObject::Instantiate<Box3D>()->GetEntityId();
}
