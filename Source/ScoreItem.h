#pragma once
#include <mtgb.h>

class ScoreItem : public mtgb::GameObject
{
  public:
	ScoreItem();
	~ScoreItem();

	void Update() override;
	void Start() override;

  private:
	Transform* pTransform_;
	Collider* pCollider_;
	RigidBody* pRigidBody_;
	MeshRenderer* pMeshRenderer_;
	uint32_t addScore_;
	static unsigned int generateCounter_;
};
