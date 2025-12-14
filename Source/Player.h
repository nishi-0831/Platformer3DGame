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
	Transform* pTransform_;
	Collider* pCollider_;
	MeshRenderer* pMeshRenderer_;
	RigidBody* pRigidBody_;
	Camera* pCamera_;
	const Transform* pCameraTransform_;
};
