#pragma once
#include <mtgb.h>
#include "IActor.h"
#include <Graphics/Shader/UVScrollShader.h>
namespace mtgb
{
	class BeltConveyor : public GameObject
	{
	  public:
		BeltConveyor();
		~BeltConveyor();

		void Update() override;

	  private:
		void OnCollisionEnter(EntityId _entityId);
		void OnCollisionExit(EntityId _entityId);
		// 自身に接地しているEntityのId
		EntityId groundedEntity_;
		Transform* pTransform_;
		MeshRenderer* pMeshRenderer_;
		Collider* pCollider_;
		RigidBody* pRigidBody_;
		IActor* pGrounedActor_;
		bool reverse_;
		float speed_;
		static unsigned int generateCounter_;
		float time_;
		Vector2 scrollDir_;
		Vector2 scrollSpeed_;
	};
} // namespace mtgb