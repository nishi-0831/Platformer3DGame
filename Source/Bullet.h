#pragma once
#include <mtgb.h>
#include "IActor.h"

/// <summary>
/// アクターによって射出される弾丸
/// </summary>
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
	/// <summary>
	/// 生成されてから破棄されるまでの時間
	/// </summary>
	float despawnTime_;
	/// <summary>
	/// 設定したコールバックを解除するハンドル
	/// </summary>
	TimerHandle despawnTimerHandle_;
};