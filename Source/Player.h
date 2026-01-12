#pragma once
#include <mtgb.h>
#include "Camera.h"
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
	void SetCamera(Camera* _pCamera);
	// IActor ã‚’ä»‹ã—ã¦ç¶™æ‰¿ã•ã‚Œã¾ã—ãŸ
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
	Camera* pCamera_;
	const Transform* pCameraTransform_;
	std::optional<FbxAnimationController> animController_;
	int hp_;
	HPViewer* pHPViewer_;
	// Invincible
	// Invincibility frames
	// –³“G‚©‚Ç‚¤‚©
	bool isInvincible_;
	// ”í’eA–³“G‚É‚È‚éŠÔ(•b)
	float invincibilityTimeSec_;
	// –³“GŠÔ’†‚ÌA•`‰æ—L–³‚ğØ‚è‘Ö‚¦‚éŠÔŠu
	float changeVisibilitySpan_;
	// –³“G‚É‚È‚Á‚Ä‚©‚ç‚ÌŒo‰ßŠÔ
	float elapsedInvincibilityTime_;
	// •`‰æ—L–³‚ğØ‚è‘Ö‚¦‚éˆ—‚Ìƒnƒ“ƒhƒ‹
	TimerHandle hTimerChangeVisibility_;
};
