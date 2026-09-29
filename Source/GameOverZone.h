#pragma once
#include <mtgb.h>

class GameOverZone : public mtgb::GameObject
{
  public:
	GameOverZone();
	~GameOverZone();
	void Start() override;

  private:
	Transform* pTransform_;
	Collider* pCollider_;
	RigidBody* pRigidBody_;
	int takeDamageAmoundOnPlayerFellout_;
	static unsigned int generateCounter_;
};