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
		static void GenerateFromJson(const nlohmann::json& _json);
		static void Generate(std::string_view _gameObjName);
		static void RegisterCommandListener(std::function<void(Command*)> _commandListener);
	private:
		GameObjectGenerator() = default;
		static GameObjectGenerator* GetInstance();
		static GameObjectGenerator* pInstance_;
		GameObjectFactory gameObjFactory_;
		std::function<void(Command*)> commandListener_;

	};
}