#pragma once
#include "Entity.h"
namespace mtgb
{
	enum class PrimitiveType
	{
		Box
	};
	class GameObjectGenerator
	{
	public:
		static EntityId GeneratePrimitive(std::function<void(Command*)> _commandListener,const ComponentFactory& _componentFactory,PrimitiveType _primitive = PrimitiveType::Box );
	private:
		GameObjectGenerator() = delete;
	};

}