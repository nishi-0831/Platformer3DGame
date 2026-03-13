#pragma once

#include <functional>
#include <string_view>
#include "GameObject.h"
#include <type_traits>
#include "GameObjectTypeRegistry.h"

namespace mtgb
{
	using CreateFunc = std::function<GameObject*()>;
	class IGameObjectRegistrar
	{
	  public:
		virtual ~IGameObjectRegistrar() = default;

		template <typename T> void Register(const std::string& _typeName)
		{
			Game::System<GameObjectTypeRegistry>().Register<T>(_typeName);
			RegisterFactory(
				_typeName,
				[]() -> GameObject*
				{
					return GameObject::Instantiate<T>();
				}
			);
		}

	  private:
		virtual void RegisterFactory(const std::string& _typeName, const CreateFunc& _creator) = 0;
	};

} // namespace mtgb