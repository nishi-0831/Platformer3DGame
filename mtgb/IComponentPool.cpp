#include "IComponentPool.h"
#include "SceneSystem.h"

std::unordered_map<mtgb::EntityId, std::set<std::type_index>> mtgb::IComponentPool::entityComponents_;
std::unordered_map<std::type_index, std::type_index> mtgb::IComponentPool::componentPoolTypeMap_;
std::unordered_map<mtgb::EntityId, mtgb::IComponentPool::ComponentIndexMap> mtgb::IComponentPool::componentIndices_;

mtgb::IComponentPool::IComponentPool()
{
}

mtgb::IComponentPool::~IComponentPool()
{
}

void mtgb::IComponentPool::RegisterComponentIndex(EntityId _entityId, const std::type_index& _typeIndex, size_t _componentIndex)
{
	componentIndices_[_entityId][_typeIndex] = _componentIndex;
}

std::optional<size_t> mtgb::IComponentPool::GetComponentIndex(EntityId _entityId, const std::type_index& _typeIndex)
{
	auto entityItr = componentIndices_.find(_entityId);
	if (entityItr == componentIndices_.end())
		return std::nullopt;

	auto indexItr = entityItr->second.find(_typeIndex);
	if (indexItr == entityItr->second.end())
		return std::nullopt;

	return indexItr->second;
}

void mtgb::IComponentPool::RegisterComponent(mtgb::EntityId _entityId, const std::type_index& _typeIndex)
{
	entityComponents_[_entityId].insert(_typeIndex);
}

void mtgb::IComponentPool::RegisterComponentPoolType(const std::type_index& _comp, const std::type_index& _pool)
{
	componentPoolTypeMap_.emplace(_comp, _pool);
}

void mtgb::IComponentPool::UnRegisterComponent(mtgb::EntityId _entityId, const std::type_index& _typeIndex)
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

void mtgb::IComponentPool::ClearEntity(mtgb::EntityId _entityId)
{
	// 全てのコンポーネントが割り当てられていないという判定にする
	entityComponents_[_entityId].clear();
}

void mtgb::IComponentPool::ClearComponentIndices(EntityId _entityId)
{
	componentIndices_.erase(_entityId);
}

std::optional<std::type_index> mtgb::IComponentPool::GetComponentPoolType(const std::type_index& _typeIndex)
{
	auto itr = componentPoolTypeMap_.find(_typeIndex);

	if (itr == componentPoolTypeMap_.end())
		return std::nullopt;

	return itr->second;
}

std::optional<std::reference_wrapper<const std::set<std::type_index>>> mtgb::IComponentPool::GetComponentTypes(EntityId _entityId)
{
	auto itr = entityComponents_.find(_entityId);
	if (itr != entityComponents_.end())
	{
		return std::cref(itr->second);
	}
	return std::nullopt;
}





void mtgb::IComponentPool::RegisterCurrentScene(const std::function<void()>& _onMove)
{
	Game::System<SceneSystem>().OnMove(_onMove);
}
