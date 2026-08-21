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

		template <typename T> void Register(const SpawnObjectDesc& _desc, bool _isUnique = false)
		{
			Game::System<GameObjectTypeRegistry>().Register<T>(_desc);
			RegisterFactory(
				_desc.typeName,
				[]() -> GameObject*
				{
					return GameObject::Instantiate<T>();
				},
				_isUnique
			);
		}

	  private:
		virtual void RegisterFactory(std::string_view, const CreateFunc& _creator, bool _isUnique) = 0;
	};

} // namespace mtgb