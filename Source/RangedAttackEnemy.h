#pragma once
#include <mtgb.h>
#include "IActor.h"
/// <summary>
/// 近づいたプレイヤーに球を飛ばす敵
/// </summary>
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
	void InitializeState();
	/// <summary>
	/// 球をチャージするエフェクトを破棄する
	/// </summary>
	void DestroyEnergyChargeEffect();

	Transform* pTransform_;
	RigidBody* pRigidBody_;
	MeshRenderer* pMeshRenderer_;
	Collider* pCollider_;
	Transform* pTargetTransform_;
	// 発見する距離
	float foundDistance_;
	// 球をチャージしてから放つまでの時間
	float attackTimer_;
	// チャージしてから経過した時間
	float elapsedTime_;
	// 弾速
	float bulletSpeed_;
	std::optional<FbxAnimationController> animController_;

	enum class STATE
	{
		WAIT,	// その場で待機
		AIMING, // 球をチャージ中
		ATTACK, // 球を撃っている
		DYING	// 倒された
	};
	mtstat::MTStat<STATE> state_;
	STATE nextState_;
	// 踏みつけたアクターに加える力
	float onStompedBounce_;
	static unsigned int generateCounter_;
	std::weak_ptr<EffectParameters> pEnergyChargeEffect_;
};