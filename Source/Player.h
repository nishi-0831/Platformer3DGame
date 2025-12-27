#pragma once
#include <mtgb.h>
#include "Camera.h"
class Player : public mtgb::GameObject , mtgb::ImGuiShowable
{
public:
	Player();
	~Player();

	void Update() override;
	void Draw() const override;
	void Start() override;
	void ShowImGui() override;
	void SetCamera(Camera* _pCamera);
private:
	void InitializeState();
	enum class STATE
	{
		IDLE,
		RUN,
		JUMP,
		FALL,
		DYING
	};
	mtstat::MTStat<STATE> state_;
	Transform* pTransform_;
	Collider* pCollider_;
	MeshRenderer* pMeshRenderer_;
	RigidBody* pRigidBody_;
	Camera* pCamera_;
	const Transform* pCameraTransform_;
	std::optional<FbxAnimationController> animController_;
};
