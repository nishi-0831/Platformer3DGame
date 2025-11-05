#pragma once

#include <functional>
#include <string_view>
#include "GameObject.h"
#include <type_traits>
#include "GameObjectTypeRegistry.h"

namespace mtgb
{
	using CreateFunc = std::function<GameObject* ()>;
	class IGameObjectRegistrar
	{
	public:
		virtual ~IGameObjectRegistrar() = default;

		template<typename T>
		void Register(std::string_view _typeName)
		{
			// TODO: 
			// - GameObjectTypeRegistryの直接呼出しを排除
			// - 今後、GameObject型を必要とするクラスが増えた場合直接呼び出すのでなく購読する仕組みを検討する(マクロやコールバックで)
			Game::System<GameObjectTypeRegistry>().Register<T>(_typeName);
			RegisterFactory(_typeName, []() -> GameObject* { return GameObject::Instantiate<T>(); });
		}

	private:
		virtual void RegisterFactory(std::string_view _typeName, CreateFunc _creator) = 0;
	};

}