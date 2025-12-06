#include "ResourceCollection.h"
#include "WindowContextResource.h"

mtgb::ResourceCollection::~ResourceCollection()
{
    Release();
}

void mtgb::ResourceCollection::Insert(const std::type_index& _key, WindowContextResource* _pResource)
{
    resourceCollection_.insert(std::make_pair(_key, _pResource));
}

void mtgb::ResourceCollection::Swap(ResourceCollection& other)
{
    std::swap(resourceCollection_, other.resourceCollection_);
}

void mtgb::ResourceCollection::Release()
{
    for (auto& [type, resource] : resourceCollection_)
    {
        SAFE_DELETE(resource);
    }
    resourceCollection_.clear();
}

mtgb::WindowContextResource*& mtgb::ResourceCollection::operator[](const std::type_index& key)
{
    return resourceCollection_[key];
}

const mtgb::WindowContextResource* mtgb::ResourceCollection::operator[](const std::type_index& key) const
{
    auto it = resourceCollection_.find(key);
    return it != resourceCollection_.end() ? it->second : nullptr;
}
