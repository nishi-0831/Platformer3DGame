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
	std::vector<IComponentMemento*> GetDefaultMementos(EntityId _entityId) const override;
	void SetCamera(Camera* _pCamera);
private:
	Transform* pTransform_;
	const Transform* pCameraTransform_;
	Camera* pCamera_;
	RigidBody* pRigidBody_;
	MeshRenderer* pMeshRenderer_;
	Collider* pCollider_;
};
