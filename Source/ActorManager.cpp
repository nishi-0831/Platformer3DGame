#include "stdafx.h"
#include "ActorManager.h"

void ActorManager::Initialize()
{
}

void ActorManager::Update()
{
}

void ActorManager::RegisterActor(IActor* _pActor)
{
	if (_pActor->GetId() == INVALID_ENTITY)
		return;
	if (_pActor == nullptr)
		return;

	for (IActor* pActor : actors_)
	{
		if (pActor->GetId() == _pActor->GetId())
			return;
	}
	actors_.push_back(_pActor);
}

void ActorManager::UnregisterActor(EntityId _id)
{
	if (_id == INVALID_ENTITY)
		return;

	for (auto itr = actors_.begin();itr != actors_.end();itr++)
	{
		if ( (*itr)->GetId() == _id)
		{
			actors_.erase(itr);
			return;
		}
	}
}

IActor* ActorManager::GetActor(EntityId _id)
{
	if (_id == INVALID_ENTITY)
		return nullptr;

	for (IActor* pActor: actors_)
	{
		if (pActor == nullptr)
			continue;

		if (pActor->GetId() == _id)
		{
			return pActor;
		}
	}
	return nullptr;
}
