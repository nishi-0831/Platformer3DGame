#pragma once
#include <functional>
#include <type_traits>
#include "GameObject.h"
#include "GameObjectTypeRegistry.h"
#include <unordered_map>
#include <string_view>


class GameObjectFactory
{
public:

	using CreateFunc = std::function<GameObject*()>;

	template<typename T>
	void Register(std::string_view _typeName);

	GameObject* Create(std::string_view _typeName) const;
private:
	std::unordered_map<std::string, CreateFunc> creators_;
};



template<typename T>
inline void GameObjectFactory::Register(std::string_view _typeName)
{
	if constexpr (std::is_base_of_v<GameObject, T>)
	{
		// Œ^î•ñ‚É‘Î‚·‚é–¼‘O‚ğ“o˜^
		Game::System<GameObjectTypeRegistry>().Register<T>(_typeName);

		// ì¬ŠÖ”‚ğ“o˜^
		creators_[std::string(_typeName)] = []()
			{
				return GameObject::Instantiate<T>();
			};
	}
	else
	{
		static_assert(false_v<T> && "GameObject‚ğŒp³‚µ‚Ä‚¢‚é•K—v‚ª‚ ‚è‚Ü‚·");
	}
}
