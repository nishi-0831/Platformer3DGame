#pragma once
#include <mtgb.h>
#include "IActor.h"
class RangedAttackEnemy : public mtgb::GameObject, public IActor
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
	void Wait();
	void Aiming();
	void Dying();
	void Shot();
	Transform* pTransform_;
	RigidBody* pRigidBody_;
	MeshRenderer* pMeshRenderer_;
	Collider* pCollider_;
	Transform* pTargetTransform_;
	float foundDistance_;
	float attackTimer_;
	float elapsedTime_;
	float bulletSpeed_;
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
	// 踏みつけたアクターに加える力
	float onStompedBounce_;
	static unsigned int generateCounter_;
	std::weak_ptr<EffectParameters> pEnergyChargeEffect_;
	void DestroyEnergyChargeEffect();
};