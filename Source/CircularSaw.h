#pragma once
#include <mtgb.h>
#include "IActor.h"
namespace mtgb
{
	class Saw : public GameObject, public IActor, public ImGuiShowable
	{
	  public:
		Saw();
		~Saw();

		void Update() override;
		void Draw() const override;
		void Start() override;
		void ShowImGui() override;

	  private:
		Transform* pTransform_;
		MeshRenderer* pMeshRenderer_;
		Collider* pCollider_;
		RigidBody* pRigidBody_;
		float rotateAngleSec_;
		float radius_;
		int takeDamageAmount_;

		// IActor を介して継承されました
		void OnStomped(IActor* _pOther) override;

		void OnHitSide(IActor* _pOther) override;

		void TakeDamage(int _damage) override;
	};
	class CircularSaw : public GameObject, public ImGuiShowable
	{
	  public:
		CircularSaw();
		~CircularSaw();

		void Update() override;
		void Draw() const override;
		void ShowImGui() override;
		void Start() override;

	  private:
		Transform* pTransform_;
		MeshRenderer* pMeshRenderer_;
		Collider* pCollider_;

		Transform* pPillarTransform_;
		MeshRenderer* pPillarMeshRenderer_;

		Saw* pSaw_;
		float sawOffset_;
		float rotateAngleSec_;
		static unsigned int generateCounter_;
	};

} // namespace mtgb