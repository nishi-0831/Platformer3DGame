#include "stdafx.h"
#include "GameObjectFactory.h"



GameObject* GameObjectFactory::Create(std::string_view _typeName) const
{
    auto itr = creators_.find(std::string(_typeName));
    if (itr == creators_.end())
        return nullptr;
    return itr->second();
}


