
#include "GameObjectGenerator.h"
#include "GameObject.h"
#include "Box3D.h"
#include "GameObjectCreateCommand.h"
#include "RegisterCommonGameObjectType.h"
#include "../Source/RegisterGameObjectType.h"
GameObjectGenerator* mtgb::GameObjectGenerator::pInstance_{ nullptr };

void mtgb::GameObjectGenerator::Initialize()
{
    if (pInstance_ == nullptr)
    {
        pInstance_ = new GameObjectGenerator();
    }
    
    // ゲームオブジェクトの作成関数、名前を登録
    mtgb::RegisterCommonGameObjectType(&pInstance_->gameObjFactory_);
    mtgb::RegisterGameObjectType(&pInstance_->gameObjFactory_);
}

void mtgb::GameObjectGenerator::Generate(GenerateType _primitive)
{
    if (GenerateType::Box == _primitive)
    {
        GameObjectCreateCommand* cmd = new GameObjectCreateCommand([]() {return GameObject::Instantiate<Box3D>(); }, Game::System<ComponentFactory>());
        GetInstance()->commandListener_(cmd);
    }
}

void mtgb::GameObjectGenerator::GenerateFromJson(const nlohmann::json& _json)
{
    nlohmann::json gameObjs = _json.at("GameObject");
    for (nlohmann::json::iterator itr = gameObjs.begin(); itr != gameObjs.end(); itr++)
    {
        nlohmann::json j = (*itr);
        // 型名を取得
        std::string classType = j["classType"].get<std::string>();

        GameObjectCreateCommand* cmd = new GameObjectCreateCommand(
            [classType]()
            {
                return GetInstance()->gameObjFactory_.Create(classType);
            },
            Game::System<ComponentFactory>(),
            j);

        // コマンドを渡す
        GetInstance()->commandListener_(cmd);
    }
}

void mtgb::GameObjectGenerator::Generate(std::string_view _gameObjName)
{
    GameObjectCreateCommand* cmd = new GameObjectCreateCommand([_gameObjName]()
        {
            return GetInstance()->gameObjFactory_.Create(_gameObjName);
        }, Game::System<ComponentFactory>());
    GetInstance()->commandListener_(cmd);
}

void mtgb::GameObjectGenerator::RegisterCommandListener(std::function<void(Command*)> _commandListener)
{
    GetInstance()->commandListener_ = _commandListener;
}

GameObjectGenerator* mtgb::GameObjectGenerator::GetInstance()
{
    if (pInstance_ == nullptr)
    {
        Initialize();
    }
    return pInstance_;
}
