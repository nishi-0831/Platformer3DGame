#include "stdafx.h"
#include "GameObjectTypeRegistry.h"

namespace
{
	/// <summary>
	/// 指定された名前のゲームオブジェクトが存在しなかった場合のデフォルト値
	/// </summary>
	const mtgb::SpawnObjectDesc defaultObjDesc { .category	  = "",
												 .displayName = "unregistered object",
												 .tooltip	  = "",
												 .typeName	  = DEFAULT_GAMEOBJECT_NAME };
} // namespace

void mtgb::GameObjectTypeRegistry::Initialize() {}

void mtgb::GameObjectTypeRegistry::Update() {}

std::string mtgb::GameObjectTypeRegistry::GetNameFromType(std::type_index _typeIndex) const
{
	auto itr = typeToDescIdx_.find(_typeIndex);
	if (itr == typeToDescIdx_.end())
		return "Unknown";
	return spawnObjectDescs_[itr->second].typeName;
}

const SpawnObjectDesc& mtgb::GameObjectTypeRegistry::GetSpawnObjectDesc(std::string_view _typeName) const
{
	auto typeItr = nameToType_.find(_typeName);
	if (typeItr == nameToType_.end())
		return defaultObjDesc;

	auto descItr = typeToDescIdx_.find(typeItr->second);
	if (descItr == typeToDescIdx_.end())
		return defaultObjDesc;

	return spawnObjectDescs_[descItr->second];
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

std::span<const SpawnObjectDesc> mtgb::GameObjectTypeRegistry::GetSpawnObjectDescs() const
{
	return spawnObjectDescs_;
}

std::vector<size_t> mtgb::GameObjectTypeRegistry::GetObjectPriorityOrder() const
{
	std::vector<size_t> order;
	for (auto [priority, idx] : priorityOrder_)
	{
		order.push_back(idx);
	}
	return order;
}

bool mtgb::GameObjectTypeRegistry::IsRegistered(std::string_view _typeName)
{
	return nameToType_.contains(_typeName);
}
