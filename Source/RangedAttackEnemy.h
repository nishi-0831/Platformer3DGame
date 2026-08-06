#pragma once
#include <mtgb.h>
#include "IActor.h"
class RangedAttackEnemy : public mtgb::GameObject, public mtgb::ImGuiShowable, public IActor
{
  public:
	RangedAttackEnemy();
	~RangedAttackEnemy();
	void Update() override;
	void Draw() const override;
	void Start() override;
	void ShowImGui() override;
	void OnStomped(IActor* _pOther) override;
	void OnHitSide(IActor* _pOther) override;
	void TakeDamage(int _damage) override;

  private:
	Transform* pTransform_;
	RigidBody* pRigidBody_;
	MeshRenderer* pMeshRenderer_;
	Collider* pCollider_;

	std::optional<FbxAnimationController> animController_;

	enum class STATE
	{
		WAIT,
		AIMING,
		ATTACK,
		DYING
	};
	void InitializeState();
	mtstat::MTStat<STATE> state_;
	STATE nextState_;
	static unsigned int generateCounter_;
};