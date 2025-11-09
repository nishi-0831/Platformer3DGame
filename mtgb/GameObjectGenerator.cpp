
#include "GameObjectGenerator.h"
#include "GameObject.h"
#include "Box3D.h"
#include "AddComponentCommand.h"
#include "GameObjectCreateCommand.h"
void mtgb::GameObjectGenerator::Generate(std::function<void(Command*)> _commandListener, const ComponentFactory& _componentFactory, GenerateType _primitive)
{
    if (GenerateType::Box == _primitive)
    {
        GameObjectCreateCommand* cmd = new GameObjectCreateCommand([]() {return GameObject::Instantiate<Box3D>(); }, _componentFactory);
        _commandListener(cmd);
    }
}

void mtgb::GameObjectGenerator::GenerateFromJson(std::function<void(Command*)> _commandListener, const ComponentFactory& _componentFactory, const GameObjectFactory& _gameObjFactory, const nlohmann::json& _json)
{
    // Œ^–¼‚ðŽæ“¾
    std::string classType = _json["classType"].get<std::string>();
    
    GameObjectCreateCommand* cmd = new GameObjectCreateCommand(
        [&_gameObjFactory,classType]() 
        {
            return _gameObjFactory.Create(classType);
        },
        _componentFactory,
        _json);

    // ƒRƒ}ƒ“ƒh‚ð“n‚·
    _commandListener(cmd);
}

void mtgb::GameObjectGenerator::Generate(std::function<void(Command*)> _commandListener, const ComponentFactory& _componentFactory, const GameObjectFactory& _gameObjFactory, std::string_view _gameObjName)
{
    GameObjectCreateCommand* cmd = new GameObjectCreateCommand([&_gameObjFactory, _gameObjName]()
        {
            return _gameObjFactory.Create(_gameObjName);
        }, _componentFactory);
    _commandListener(cmd);
}
