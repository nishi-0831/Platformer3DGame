#pragma once
#include <mtgb.h>

class Player : public mtgb::GameObject , mtgb::ImGuiShowable
{
public:
	Player();
	~Player();

	void Update() override;
	void Draw() const override;

private:
	Transform* pTransform_;
	RigidBody* pRigidBody_;
	MeshRenderer* pMeshRenderer_;
	Collider* pCollider_;
};
