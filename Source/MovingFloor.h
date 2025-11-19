#pragma once
#include <mtgb.h>
#include "MovingFloorData.h"
#include "MovingFloorCP.h"
class MovingFloorCP;
class MovingFloor : public StatefulComponent<MovingFloor, MovingFloorCP, MovingFloorData>
{
public:
	using StatefulComponent<MovingFloor, MovingFloorCP, MovingFloorData>::StatefulComponent;
	friend MovingFloorCP;
	MovingFloor();
	MovingFloor(EntityId _entityId);
	
	void Update();
protected:
	void OnPostRestore() override;
private:
	void OnCollisionEnter(EntityId _entityId);
	float dir_;
	float elapsed_;
	Transform* pTransform_;
	MeshRenderer* pMeshRenderer_;
	Collider* pCollider_;
	RigidBody* pRigidBody_;
	Transform* pToTransform_;
	Transform* pFromTransform_;
	static unsigned int generateCounter_;
};