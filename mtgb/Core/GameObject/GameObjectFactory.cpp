#include "stdafx.h"
#include "GameObjectFactory.h"
#include "Core/SceneSystem.h"
mtgb::GameObject* mtgb::GameObjectFactory::Create(std::string_view _typeName) const
{
	if (uniqueGameObjectNames_.contains(_typeName))
	{
		GameObject* gameObj = Game::System<SceneSystem>().GetActiveScene()->GetGameObject(_typeName);
		if (gameObj != nullptr)
		{
			return nullptr;
		}
	}

	auto itr = creators_.find(_typeName);
	if (itr == creators_.end())
		return nullptr;

	GameObject* gameObj = itr->second();
	return gameObj;
}

void mtgb::GameObjectFactory::RegisterFactory(std::string_view _typeName, const CreateFunc& _creator, bool _isUnique)
{
	creators_.emplace(_typeName, _creator);
	if (_isUnique)
	{
		uniqueGameObjectNames_.emplace(_typeName);
	}
}
