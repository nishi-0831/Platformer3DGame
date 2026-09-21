#pragma once
#include <mtgb.h>
#include <stack>
#include "IActor.h"
namespace mtgb
{
	class DamageObject : public GameObject, public IActor
	{
	  public:
		DamageObject();
		~DamageObject();

		void Update() override;
		void Draw() const override;
		void Start() override;

		Transform* pTransform_;
		MeshRenderer* pMeshRenderer_;
		Collider* pCollider_;
		RigidBody* pRigidBody_;

	  private:
		int takeDamageAmount_;
		// IActor を介して継承されました
		void OnStomped(IActor* _pOther) override;

		void OnHitSide(IActor* _pOther) override;

		void TakeDamage(int _damage) override;
	};
	class RotateDamageBar : public GameObject
	{
	  public:
		RotateDamageBar();
		~RotateDamageBar();

		void Update() override;
		void Draw() const override;
		void ShowImGui() override;
		void Start() override;
		void StartOnEditMode() override;
		void AddSpike();
		void RemoveSpike();
		nlohmann::json Serialize() const override;
		void Deserialize(const nlohmann::json& _json) override;

	  private:
		Transform* pTransform_;
		MeshRenderer* pMeshRenderer_;
		Collider* pCollider_;

		std::stack<DamageObject*> pDamageObjs_;
		// 一秒あたりに回転させる角度
		float rotateAngleSec_;
		int spikeCount_;
		float spikeRadius_;
		static unsigned int generateCounter_;
	};

} // namespace mtgb