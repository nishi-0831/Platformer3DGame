#pragma once
#include "Entity.h"
#include "Command.h"
#include "ComponentFactory.h"
#include "GameObjectFactory.h"
namespace mtgb
{
	enum class GenerateType
	{
		Box
	};
	class GameObjectGenerator
	{
	public:

		static void Initialize();
		static void Generate(GenerateType _primitive);
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
		/// コマンドを渡すことができるリスナーを登録
		/// </summary>
		static void RegisterCommandListener(std::function<void(Command*)> _commandListener);
		static void Duplicate(EntityId _srcEntityId);
	private:
		GameObjectGenerator() = default;
		static GameObjectGenerator* GetInstance();
		static GameObjectGenerator* pInstance_;
		GameObjectFactory gameObjFactory_;
		std::function<void(Command*)> commandListener_;

	};
}