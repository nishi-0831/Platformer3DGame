#pragma once
#include "Entity.h"
#include "Command.h"
#include "ComponentFactory.h"
namespace mtgb
{
	enum class PrimitiveType
	{
		Box
	};
	class GameObjectGenerator
	{
	public:
		static void GeneratePrimitive(std::function<void(Command*)> _commandListener,const ComponentFactory& _componentFactory,PrimitiveType _primitive = PrimitiveType::Box );
	private:
		GameObjectGenerator() = delete;
	};

}