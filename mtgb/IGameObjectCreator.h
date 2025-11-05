#pragma once
#include <string_view>
namespace mtgb
{
	class GameObject;
	class IGameObjectCreator
	{
	public:
		IGameObjectCreator() = default;
		virtual ~IGameObjectCreator() = default;
		virtual GameObject* Create(std::string_view _typeName) const = 0;
	};
}
