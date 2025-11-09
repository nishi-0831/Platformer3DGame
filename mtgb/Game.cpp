#include "Game.h"
#include "ComponentPool.h"
//#include "Windows.h"
#include "IComponentPool.h"
#include "Entity.h"

namespace
{
	static const mtgb::Vector2Int DEFAULT_SCREEN_SIZE{ 800, 600 };
}

mtgb::Game::Game() :
	pRegisterSystems_{},
	pFrameUpdateSystems_{},
	pFixedUpdateSystems_{},
	pComponentPools_{}
{
}

mtgb::Game::~Game()
{
}

mtgb::Vector2Int mtgb::Game::GetScreenSize() const
{
	return DEFAULT_SCREEN_SIZE;
}

void mtgb::Game::Exit()
{
	toExit_ = true;
}

void mtgb::Game::UpdateFrame()
{
	for (auto&& updateSystem : pInstance_->pFrameUpdateSystems_)
	{
		updateSystem->Update();
	}
}

void mtgb::Game::UpdateFixed()
{
	for (auto&& updateSystem : pInstance_->pFixedUpdateSystems_)
	{
		updateSystem->Update();
	}
}

void mtgb::Game::RemoveEntityAllComponent(const EntityId _entityId)
{
	for (auto&& cpSystem : pInstance_->pComponentPools_)
	{
		cpSystem->Remove(_entityId);
	}
}

void mtgb::Game::RemoveEntityComponent(const std::type_index _typeIndex, EntityId _entityId)
{
	auto itr = pInstance_->pRegisterSystems_.find(_typeIndex);

	if (itr == pInstance_->pRegisterSystems_.end())
		return;

	IComponentPool* pComponentPool = dynamic_cast<IComponentPool*>(itr->second);

	if (pComponentPool == nullptr) return;

	pComponentPool->Remove(_entityId);

}

nlohmann::json mtgb::Game::SerializeComponent(std::type_index _typeIndex, EntityId _entityId)
{
	auto itr = pInstance_->pRegisterSystems_.find(_typeIndex);
	nlohmann::json j;

	if (itr == pInstance_->pRegisterSystems_.end())
		return j;

	IComponentPool* pComponentPool = dynamic_cast<IComponentPool*>(itr->second);

	if (pComponentPool != nullptr)
	{
		j = pComponentPool->Serialize(_entityId);
	}

	return j;
}

IComponentMemento* mtgb::Game::DeserializeComponent(std::type_index _typeIndex, EntityId _entityId, const nlohmann::json& _json)
{
	auto itr = pInstance_->pRegisterSystems_.find(_typeIndex);
	if (itr == pInstance_->pRegisterSystems_.end())
		return nullptr;

	IComponentPool* pComponentPool = dynamic_cast<IComponentPool*>(itr->second);
	if (pComponentPool == nullptr)
		return nullptr;

	return pComponentPool->Deserialize(_entityId,_json);
}

std::optional<std::vector<IComponentMemento*>> mtgb::Game::DeserializeComponents(EntityId _entityId, const nlohmann::json& _json)
{
	std::vector<IComponentMemento*> ret;

	std::optional<std::set<std::type_index>> components = Game::System<ComponentRegistry>().GetComponentTypes(_json);
	if (components.has_value() == false)
		return std::nullopt;

	for (const std::type_index& typeIndex : components.value())
	{
		std::optional<std::type_index> componentPoolType = Game::System<ComponentRegistry>().GetComponentPoolType(typeIndex);

		if (componentPoolType.has_value() == false)
			continue;

		ret.emplace_back(Game::DeserializeComponent(componentPoolType.value(), _entityId, _json));
	}

	return std::optional<std::vector<IComponentMemento*>>(std::move(ret));
}

std::span<IRenderableCP*> mtgb::Game::GetRenderableCPs()
{
	return { pInstance_->pRenderablePools_.data(), pInstance_->pRenderablePools_.size()};
}



void mtgb::Game::InitializeSystems(const std::list<ISystem*>& _uninitialized)
{
	for (auto itr = pInstance_->registerOrder_.begin(); itr != pInstance_->registerOrder_.end(); itr++)
	{
		pInstance_->pRegisterSystems_[*itr]->Initialize();
	}
}

void mtgb::Game::ReleaseSystems(const std::list<ISystem*>& _runnings)
{
	for (auto itr = pInstance_->registerOrder_.rbegin(); itr != pInstance_->registerOrder_.rend(); itr++)
	{
		ISystem* pSystem = pInstance_->pRegisterSystems_[*itr];
		pSystem->Release();
		delete pSystem;
	}

	/*for (auto itr = _runnings.rbegin(); itr != _runnings.rend(); itr++)
	{
		(*itr)->Release();
		delete (*itr);
	}*/
}

void mtgb::Game::RunLoopGameCycle()
{
	while (true)
	{
		for (auto&& updateSystem : pInstance_->pCycleUpdateSystems_)
		{
			updateSystem->Update();
		}

		if (toExit_)  // 終了フラグが立っていたらサイクル離脱
		{
			break;
		}
	}
}

mtgb::Game* mtgb::Game::pInstance_{ nullptr };
bool mtgb::Game::toExit_{ false };
