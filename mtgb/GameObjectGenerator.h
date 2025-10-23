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
		static EntityId GeneratePrimitive(PrimitiveType _primitive = PrimitiveType::Box );
	private:
		GameObjectGenerator() = delete;
	};

}