
#include "GameObjectGenerator.h"
#include "GameObject.h"
#include "Box3D.h"
#include "GameObjectCreateCommand.h"
#include "RegisterCommonGameObjectType.h"
#include "../Source/RegisterGameObjectType.h"
#include "DuplicateGameObjectCommand.h"
#include "CommandHistoryManager.h"
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
        GameObjectCreateCommand* cmd = new GameObjectCreateCommand([]() {return GameObject::Instantiate<Box3D>(); }, Game::GetComponentFactory());
        GetInstance()->commandListener_(cmd);
    }
}

void mtgb::GameObjectGenerator::GenerateFromJson(const nlohmann::json& _json)
{
    CommandHistoryManager& cmdHitoryManager = Game::System<CommandHistoryManager>();
    cmdHitoryManager.BeginGroupCommand();

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
            Game::GetComponentFactory(),
            j);
        
        // コマンドを渡す
        GetInstance()->commandListener_(cmd);
        // コンポーネントのデシリアライズ
        Game::DeserializeComponents(cmd->GetCommandTargetEntityId(), j);
    }
    cmdHitoryManager.EndGroupCommand();
}

void mtgb::GameObjectGenerator::Generate(std::string_view _gameObjName)
{
    GameObjectCreateCommand* cmd = new GameObjectCreateCommand([_gameObjName]()
        {
            return GetInstance()->gameObjFactory_.Create(_gameObjName);
        }, Game::GetComponentFactory());
    GetInstance()->commandListener_(cmd);
}

void mtgb::GameObjectGenerator::RegisterCommandListener(std::function<void(Command*)> _commandListener)
{
    GetInstance()->commandListener_ = _commandListener;
}

void mtgb::GameObjectGenerator::Duplicate(EntityId _srcEntityId)
{
    // TODO:ゲームオブジェクト作成、コンポーネントの付与のコマンドをまとめる
    GameObject* src = Game::System<SceneSystem>().GetActiveScene()->GetGameObject(_srcEntityId);
    std::string classTypeName = src->GetClassTypeName();

    DuplicateGameObjectCommand* cmd = new DuplicateGameObjectCommand([classTypeName]()
        {
            return GetInstance()->gameObjFactory_.Create(classTypeName);
        }, Game::GetComponentFactory(), _srcEntityId);
    GetInstance()->commandListener_(cmd);
}

GameObjectGenerator* mtgb::GameObjectGenerator::GetInstance()
{
    if (pInstance_ == nullptr)
    {
        Initialize();
    }
    return pInstance_;
}
