#pragma once
#include <mtgb.h>
#include "MovingFloorState.h"
#include "MovingFloorCP.h"
class MovingFloorCP;
class MovingFloor : public StatefulComponent<MovingFloor, MovingFloorCP, MovingFloorState>
{
public:
	using StatefulComponent<MovingFloor, MovingFloorCP, MovingFloorState>::StatefulComponent;
	friend MovingFloorCP;
	MovingFloor();
	MovingFloor(EntityId _entityId);
	
	void Update();
	
	
	void UpdateProgress();
	
	Vector3 Evaluate();
protected:
	void OnPostRestore() override;
	void OnPreSave() override;
private:
	void OnCollisionEnter(EntityId _entityId);
	void OnCollisionExit(EntityId _entityId);
	// 進行方向
	float dir_;
	// 経過時間
	float elapsed_;
	// このコンポーネントが割り当てられているEntityに着地しているEntityのId
	EntityId groundedEntity_;
	Transform* pTransform_;
	MeshRenderer* pMeshRenderer_;
	Collider* pCollider_;
	RigidBody* pRigidBody_;
	Transform* pToTransform_;
	Transform* pFromTransform_;
	static unsigned int generateCounter_;
};