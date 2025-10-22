#include "IComponentPool.h"
#include "SceneSystem.h"

std::unordered_map<mtgb::EntityId, std::set<std::type_index>> mtgb::IComponentPool::entityComponents_;
mtgb::IComponentPool::IComponentPool()
{
}

mtgb::IComponentPool::~IComponentPool()
{
}

void mtgb::IComponentPool::RegisterComponent(mtgb::EntityId _entityId, const std::type_index& _typeIndex)
{
	entityComponents_[_entityId].insert(_typeIndex);
}

void mtgb::IComponentPool::UnRegisterComponent(mtgb::EntityId _entityId, const std::type_index& _typeIndex)
{
	auto itr = entityComponents_.find(_entityId);
	if (itr != entityComponents_.end())
	{
		// �R���|�[�l���g�����蓖�Ă��Ă��Ȃ��Ƃ�������ɂ���
		itr->second.erase(_typeIndex);

		// Entity �ɃR���|�[�l���g��������蓖�Ă��ĂȂ��Ȃ�폜
		if (itr->second.empty())
		{
			entityComponents_.erase(itr);
		}
	}
}

void mtgb::IComponentPool::ClearEntity(mtgb::EntityId _entityId)
{
	// �S�ẴR���|�[�l���g�����蓖�Ă��Ă��Ȃ��Ƃ�������ɂ���
	entityComponents_[_entityId].clear();
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
