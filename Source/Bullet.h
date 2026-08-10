#pragma once
#include <mtgb.h>
#include "IActor.h"

class Bullet : public mtgb::GameObject, public IActor
{
  public:
	Bullet(EntityId _gunnerId);
	~Bullet();
	void OnStomped(IActor* _pOther) override;
	void OnHitSide(IActor* _pOther) override;
	void TakeDamage(int _damage) override;

  private:
	void OnHit(IActor* _pOther);
	EntityId gunnerId_;
	MeshRenderer* pMeshRenderer_;
	RigidBody* pRigidBody_;
	int takeDamageAmount_;
	float despawnTime_;
	TimerHandle despawnTimerHandle_;
};