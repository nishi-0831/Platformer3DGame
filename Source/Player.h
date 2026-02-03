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
	// IActor を介して継承されました
	void OnStomped(IActor* _pOther) override;
	void OnHitSide(IActor* _pOther) override;
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
};
