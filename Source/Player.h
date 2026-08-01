#pragma once
#include <mtgb.h>
#include "SphericalCamera.h"
#include "QuaternionCamera.h"
#include "IActor.h"
#include "HPViewer.h"
#include "JumpController.h"

class Player : public mtgb::GameObject, public IActor
{
  public:
	Player();
	~Player();

	void Update() override;
	void Draw() const override;
	void Start() override;
	void ShowImGui() override;
	// IActor を介して継承されました
	void OnStomped(IActor* _pOther) override;
	void OnHitSide(IActor* _pOther) override;
	void TakeDamage(int _damage) override;

  private:
	Vector3 GetMoveDir();
	void UpdatePosition();
	void UpdateRotate();
	void OnCollisionEnter(EntityId _entityId);
	void OnFootstep(const AnimationEvent& _event);
	void InitializeState();
	enum class STATE
	{
		IDLE,
		RUN,
		JUMP,
		FALL,
		DYING,
		VICTORY
	};
	mtstat::MTStat<STATE> state_;
	Transform* pTransform_;
	Collider* pCollider_;
	MeshRenderer* pMeshRenderer_;
	RigidBody* pRigidBody_;
	QuaternionCamera* pCamera_;
	const Transform* pCameraTransform_;
	std::optional<FbxAnimationController> animController_;
	int hp_;
	HPViewer* pHPViewer_;
	// 無敵かどうか
	bool isInvincible_;
	// 被弾時、無敵になる時間(秒)
	float invincibilityTimeSec_;
	// 無敵時間中の、描画有無を切り替える間隔
	float changeVisibilitySpan_;
	// 無敵になってからの経過時間
	float elapsedInvincibilityTime_;
	// 描画有無を切り替える処理のハンドル
	TimerHandle hTimerChangeVisibility_;
	JumpController jumpController_;

	// 歩いている際に煙のエフェクトを再生する間隔
	float walkSmokeInterval_;
	// 煙のエフェクトを出す間隔を計る経過時間
	float walkSmokeElapsedTime_;
	float jumpHeight_;
	float moveSpeed_;
};