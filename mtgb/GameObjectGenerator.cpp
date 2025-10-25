#include "GameObjectGenerator.h"
#include "GameObject.h"
#include "Box3D.h"

EntityId mtgb::GameObjectGenerator::GeneratePrimitive(std::function<void(Command*)> _commandListener, const ComponentFactory& _componentFactory, PrimitiveType _primitive)
{
    return  GameObject::Instantiate<Box3D>()->GetEntityId();
}
