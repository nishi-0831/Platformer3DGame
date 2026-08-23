#include "Core/EntityManager.h"
#include <algorithm>
void mtgb::EntityManager::Initialize()
{
	entityCounter_ = INVALID_ENTITY;
}

void mtgb::EntityManager::Update() {}

mtgb::EntityId mtgb::EntityManager::CreateEntity()
{
	return ++entityCounter_;
}

mtgb::EntityId mtgb::EntityManager::GetCounter()
{
	return entityCounter_;
}

void mtgb::EntityManager::ChangeCounter(EntityId _counter)
{
	entityCounter_ = _counter;
}

void mtgb::EntityManager::DecrementCounter()
{
	entityCounter_--;
	entityCounter_ = (std::max)(entityCounter_, INVALID_ENTITY + 1);
}
