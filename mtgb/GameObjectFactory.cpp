#include "stdafx.h"
#include "GameObjectFactory.h"



mtgb::GameObject* mtgb::GameObjectFactory::Create(std::string_view _typeName) const
{
    auto itr = creators_.find(std::string(_typeName));
    if (itr == creators_.end())
        return nullptr;
    return itr->second();
}

void mtgb::GameObjectFactory::RegisterFactory(std::string_view _typeName, CreateFunc _creator)
{
    creators_.emplace(std::string(_typeName), std::move(_creator));
}


