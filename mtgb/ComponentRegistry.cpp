#include "ComponentRegistry.h"

void mtgb::ComponentRegistry::Initialize()
{
}

void mtgb::ComponentRegistry::Update()
{
}

mtgb::ComponentRegistry::ComponentRegistry()
{
}

void mtgb::ComponentRegistry::RegisterComponentIndex(EntityId _entityId, const std::type_index& _typeIndex, size_t _componentIndex)
{
	componentIndices_[_entityId][_typeIndex] = _componentIndex;
}

std::optional<size_t> mtgb::ComponentRegistry::GetComponentIndex(EntityId _entityId, const std::type_index& _componentTypeIndex)
{
	// EntityIdに割り当てられているコンポーネント:インデックスのマップを取得
	auto entityItr = componentIndices_.find(_entityId);
	if (entityItr == componentIndices_.end())
		return std::nullopt;

	// コンポーネントのインデックスを取得
	auto indexItr = entityItr->second.find(_componentTypeIndex);
	if (indexItr == entityItr->second.end())
		return std::nullopt;

	return indexItr->second;
}

void mtgb::ComponentRegistry::RegisterComponent(mtgb::EntityId _entityId, const std::type_index& _componentTypeIndex)
{
	entityComponents_[_entityId].insert(_componentTypeIndex);
}

void mtgb::ComponentRegistry::RegisterComponentKey(const std::string& _key, const std::type_index& _typeIndex)
{
	componentNameToTypeMap_.emplace(_key, _typeIndex);
}

void mtgb::ComponentRegistry::RegisterComponentPoolType(const std::type_index& _comp, const std::type_index& _pool)
{
	componentTypeToPoolTypeMap_.emplace(_comp, _pool);
}

void mtgb::ComponentRegistry::UnRegisterComponent(mtgb::EntityId _entityId, const std::type_index& _typeIndex)
{
	auto itr = entityComponents_.find(_entityId);
	if (itr != entityComponents_.end())
	{
		// コンポーネントが割り当てられていないという判定にする
		itr->second.erase(_typeIndex);

		// Entity にコンポーネントが一つも割り当てられてないなら削除
		if (itr->second.empty())
		{
			entityComponents_.erase(itr);
		}
	}
}

void mtgb::ComponentRegistry::ClearEntity(mtgb::EntityId _entityId)
{
	// 全てのコンポーネントが割り当てられていないという判定にする
	entityComponents_[_entityId].clear();
}

void mtgb::ComponentRegistry::ClearComponentIndices(EntityId _entityId)
{
	componentIndices_.erase(_entityId);
}

std::optional<std::type_index> mtgb::ComponentRegistry::GetComponentPoolType(const std::type_index& _componentTypeIndex)
{
	auto itr = componentTypeToPoolTypeMap_.find(_componentTypeIndex);

	if (itr == componentTypeToPoolTypeMap_.end())
		return std::nullopt;

	return itr->second;
}

std::optional<std::type_index> mtgb::ComponentRegistry::GetComponentPoolType(const std::string& _componentName)
{
	auto itr = componentNameToTypeMap_.find(_componentName);

	if (itr == componentNameToTypeMap_.end())
		return std::nullopt;

	return GetComponentPoolType(itr->second);
}

std::optional<std::reference_wrapper<const std::set<std::type_index>>> mtgb::ComponentRegistry::GetComponentTypes(EntityId _entityId)
{
	auto itr = entityComponents_.find(_entityId);

	if (itr == entityComponents_.end())
		return std::nullopt;
	
	return std::cref(itr->second);
}

std::optional<std::set<std::type_index>> mtgb::ComponentRegistry::GetComponentTypes(const nlohmann::json& _json)
{
	std::set<std::type_index> components;
	for (auto& itr : componentNameToTypeMap_)
	{
		auto [name, typeIndex] = itr;
		if (_json.contains(name))
		{
			components.insert(typeIndex);
		}
	}

	if (components.empty())
		return std::nullopt;

	// 明示的にムーブする
	return std::optional<std::set<std::type_index>>(std::move(components));
}
