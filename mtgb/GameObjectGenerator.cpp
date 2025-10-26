#include "GameObjectGenerator.h"
#include "GameObject.h"
#include "Box3D.h"
#include "AddComponentCommand.h"
#include "GameObjectCreateCommand.h"
void mtgb::GameObjectGenerator::GeneratePrimitive(std::function<void(Command*)> _commandListener, const ComponentFactory& _componentFactory, PrimitiveType _primitive)
{
    if (PrimitiveType::Box == _primitive)
    {

        GameObjectCreateCommand* cmd = new GameObjectCreateCommand([]() {return GameObject::Instantiate<Box3D>(); }, _componentFactory);
        _commandListener(cmd);

        /*Box3D* box = GameObject::Instantiate<Box3D>();
        auto mementos = box->GetDefaultMementos(box->GetEntityId());

        for (mtgb::IComponentMemento* memento : mementos)
        {
            _commandListener( new AddComponentCommand(box->GetEntityId(), memento->GetComponentType(), memento, _componentFactory));
        }*/
        
    }
    
}
