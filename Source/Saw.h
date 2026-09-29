#pragma once
#include <mtgb.h>
#include "IActor.h"
namespace mtgb
{
	class Saw : public GameObject, public IActor
	{
	  public:
		Saw();
		~Saw();

		void Update() override;
		void OnPreDrawScene() const override;
		void Start() override;
		void ShowImGui() override;

	  protected:
		Transform* pTransform_;
		MeshRenderer* pMeshRenderer_;
		Collider* pCollider_;
		RigidBody* pRigidBody_;
		float rotateAngleSec_;
		float radius_;
		int takeDamageAmount_;
		int audioSourceHandle_;
		// IActor を介して継承されました
		void OnStomped(IActor* _pOther) override;

		void OnHitSide(IActor* _pOther) override;

		void TakeDamage(int _damage) override;
	};
} // namespace mtgb