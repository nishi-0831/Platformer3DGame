#include "stdafx.h"
#include "GameObjectTypeRegistry.h"

void mtgb::GameObjectTypeRegistry::Initialize()
{
}

void mtgb::GameObjectTypeRegistry::Update()
{
}

std::vector<std::string> mtgb::GameObjectTypeRegistry::GetRegisteredNames() const
{
    std::vector<std::string> names;
    names.reserve(nameToType_.size());
    for (const auto& keyValue : nameToType_)
    {
        names.emplace_back(keyValue.first);
    }

    std::sort(names.begin(), names.end());

    return names;
}
