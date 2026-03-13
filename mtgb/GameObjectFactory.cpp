#include "stdafx.h"
#include "GameObjectFactory.h"
#include "EventManager.h"
#include "GameObjectSelectionEvent.h"

mtgb::GameObject* mtgb::GameObjectFactory::Create(std::string_view _typeName) const
{
	auto itr = creators_.find(_typeName);
	if (itr == creators_.end())
		return nullptr;

	GameObject* gameObj = itr->second();
	return gameObj;
}

void mtgb::GameObjectFactory::RegisterFactory(const std::string& _typeName, const CreateFunc& _creator)
{
	creators_.emplace(_typeName, _creator);
}
