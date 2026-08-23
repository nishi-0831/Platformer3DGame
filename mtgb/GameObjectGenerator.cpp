
#include "GameObjectGenerator.h"
#include "GameObject.h"
#include "Editor/Command/GameObjectCreateCommand.h"
#include "RegisterCommonGameObjectType.h"
#include "../Source/RegisterGameObjectType.h"
#include "Editor/Command/DuplicateGameObjectCommand.h"
#include "Editor/Command/CommandHistoryManager.h"
#include "Editor/Command/DeleteGameObjectCommand.h"

mtgb::GameObjectGenerator* mtgb::GameObjectGenerator::pInstance_ { nullptr };

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
			j
		);

		// コマンドを渡す
		cmdHitoryManager.ExecuteCommand(cmd);
		// コンポーネントのデシリアライズ
		Game::DeserializeComponents(cmd->GetCommandTargetEntityId(), j);
	}
	cmdHitoryManager.EndGroupCommand();
}

void mtgb::GameObjectGenerator::Generate(std::string_view _gameObjName)
{
	std::string gameObjName { _gameObjName };
	GameObjectCreateCommand* cmd = new GameObjectCreateCommand(
		[gameObjName]()
		{
			return GetInstance()->gameObjFactory_.Create(gameObjName);
		}
	);
	Game::System<CommandHistoryManager>().ExecuteCommand(cmd);
}

void mtgb::GameObjectGenerator::Duplicate(std::span<EntityId> _srcEntityIds)
{
	// 複製コマンドを作成、実行
	DuplicateGameObjectCommand* cmd = new DuplicateGameObjectCommand(_srcEntityIds, GetInstance()->gameObjFactory_);
	Game::System<CommandHistoryManager>().ExecuteCommand(cmd);
}

void mtgb::GameObjectGenerator::Delete(std::span<EntityId> _entityIds)
{
	// 削除コマンドを作成、実行
	DeleteGameObjectCommand* cmd = new DeleteGameObjectCommand(_entityIds, GetInstance()->gameObjFactory_);
	Game::System<CommandHistoryManager>().ExecuteCommand(cmd);
}

mtgb::GameObjectGenerator* mtgb::GameObjectGenerator::GetInstance()
{
	if (pInstance_ == nullptr)
	{
		Initialize();
	}
	return pInstance_;
}
