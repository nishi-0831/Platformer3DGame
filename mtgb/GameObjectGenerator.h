#pragma once
#include "Entity.h"
#include "Command.h"
#include "ComponentFactory.h"
#include "GameObjectFactory.h"
namespace mtgb
{
	class GameObjectGenerator
	{
	  public:
		static void Initialize();
		/// <summary>
		/// JSONからゲームオブジェクトを生成する
		/// </summary>
		/// <param name="_gameObjName"></param>
		static void GenerateFromJson(const nlohmann::json& _json);
		/// <summary>
		/// 名前に対応したゲームオブジェクトを生成する
		/// </summary>
		/// <returns></returns>
		static void Generate(std::string_view _gameObjName);
		/// <summary>
		/// ゲームオブジェクトを複製する
		/// </summary>
		static void Duplicate(EntityId _srcEntityId);
		/// <summary>
		/// ゲームオブジェクトを削除する
		/// </summary>
		/// <param name="_entityId"></param>
		static void Delete(EntityId _entityId);

	  private:
		GameObjectGenerator() = default;
		static GameObjectGenerator* GetInstance();
		static GameObjectGenerator* pInstance_;
		GameObjectFactory gameObjFactory_;
	};
} // namespace mtgb