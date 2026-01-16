#pragma once
#include <mtgb.h>
#include "SphericalCamera.h"
#include "QuaternionCamera.h"
#include "IActor.h"
#include "HPViewer.h"

class Player : public mtgb::GameObject, public mtgb::ImGuiShowable, public IActor
{
  public:
	Player();
	~Player();

	void Update() override;
	void Draw() const override;
	void Start() override;
	void ShowImGui() override;
	void SetCamera(SphericalCamera* _pCamera);
	// IActor 繧剃ｻ九＠縺ｦ邯呎価縺輔ｌ縺ｾ縺励◆
	void OnStomped(IActor* pOther) override;
	void OnHitSide(IActor* pOther) override;
	void TakeDamage(int _damage) override;

  private:
	Vector3 GetMoveDir();
	void UpdatePosition();
	void UpdateRotate();
	void OnCollisionEnter(EntityId _entityId);
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
	QuaternionCamera* pNewCamera_;
	SphericalCamera* pCamera_;
	const Transform* pCameraTransform_;
	std::optional<FbxAnimationController> animController_;
	int hp_;
	HPViewer* pHPViewer_;
	// Invincible
	// Invincibility frames
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
};
