#pragma once
#include <mtgb.h>

class MovingFloor : public GameObject , public ImGuiShowable
{
public:
	MovingFloor();
	void Update() override;
	void ShowImGui() override;
private:
	void OnCollisionEnter(EntityId _entityId);
	void OnCollisionExit(EntityId _entityId);
	// このコンポーネントが割り当てられているEntityに着地しているEntityのId
	EntityId groundedEntity_;
	Transform* pTransform_;
	MeshRenderer* pMeshRenderer_;
	Collider* pCollider_;
	RigidBody* pRigidBody_;
	Interpolator* pInterpolator_;
	static unsigned int generateCounter_;
};