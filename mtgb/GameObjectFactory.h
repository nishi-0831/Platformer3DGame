#pragma once
#include <functional>
#include <type_traits>
#include "GameObject.h"
#include "IGameObjectCreator.h"
#include "IGameObjectRegistrar.h"
#include "GameObjectTypeRegistry.h"
#include <unordered_map>
#include <string_view>

namespace mtgb
{
	class GameObjectFactory :  public IGameObjectCreator , public IGameObjectRegistrar
	{
	public:
		GameObjectFactory() = default;
		

		GameObject* Create(std::string_view _typeName) const;
		void RegisterFactory(std::string_view _typeName, CreateFunc _creator) override;
	private:
		std::unordered_map<std::string, CreateFunc> creators_;
	};
}