#pragma once
#include "Entity.h"
class IActor
{
public:
	IActor(EntityId _entityId);
	virtual ~IActor();
	virtual void OnStomped(IActor* _pOther) = 0;
	virtual void OnHitSide(IActor* _pOther) = 0;
	virtual void TakeDamage(int _damage) = 0;
	EntityId GetId() { return id_; }
private:
	IActor() = delete;
	EntityId id_;
};