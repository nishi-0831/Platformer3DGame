#pragma once
#include "ReflectionMacro.h"
#include <mtgb.h>
#include "MovingFloor.generated.h"
#include "../mtgb/ISerializableObject.h"
#include "MovingFloorState.h"
#include "MovingFloorCP.h"

class MovingFloorCP;
MT_COMPONENT()
class MovingFloor : public IComponent<MovingFloorCP,MovingFloor> , public ISerializableObject
{
public:
	MT_GENERATED_BODY()

	friend MovingFloorCP;
	using IComponent::IComponent;
	MovingFloor();
	MovingFloor(EntityId _entityId);
	
	void Update();
	
	
	void UpdateProgress();
	
	Vector3 Evaluate();
	void OnPostRestore() override;
	void OnPreSave() override;
protected:
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

	MT_PROPERTY()
	// 補間にかかる時間
	float duration_;
	MT_PROPERTY()
	Vector3 to_;
	MT_PROPERTY()
	Vector3 from_;

	static unsigned int generateCounter_;
};