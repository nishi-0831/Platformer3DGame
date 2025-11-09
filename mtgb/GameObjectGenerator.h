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
		static void Generate(std::function<void(Command*)> _commandListener,const ComponentFactory& _componentFactory,GenerateType _primitive);
		static void GenerateFromJson(std::function<void(Command*)> _commandListener, const ComponentFactory& _componentFactory, const GameObjectFactory& _gameObjFactory,const nlohmann::json& _json);
		static void Generate(std::function<void(Command*)> _commandListener, const ComponentFactory& _componentFactory, const
			GameObjectFactory& _gameObjFactory, std::string_view _gameObjName);
	private:
		GameObjectGenerator() = delete;
	};
}