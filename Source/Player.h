#pragma once
#include <mtgb.h>

class Player : public mtgb::GameObject , mtgb::ImGuiShowable
{
public:
	Player();
	~Player();

	void Update() override;
	void Draw() const override;
	void ShowImGui() override;
	void SetCamera(CameraHandleInScene _hCamera);
private:
	Transform* pTransform_;
	const Transform* pCameraTransform_;
	RigidBody* pRigidBody_;
	MeshRenderer* pMeshRenderer_;
	Collider* pCollider_;
};
