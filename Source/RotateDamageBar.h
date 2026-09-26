#pragma once
#include <mtgb.h>
#include <stack>
#include <tuple>
#include "IActor.h"
#include "Editor/SpinBox.h"
namespace mtgb
{
	class DamageObject : public GameObject, public IActor
	{
	  public:
		DamageObject();
		~DamageObject();

		void Update() override;
		void OnPreDrawScene() const override;
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
		void OnPreDrawScene() const override;
		void ShowImGui() override;
		void Start() override;
		void StartOnEditMode() override;
		void AddSpike();
		void RemoveSpike();
		nlohmann::json SerializeProperties() const override;
		void DeserializeProperties(const nlohmann::json& _json) override;

	  private:
		void RotateInitialAngle();
		Transform* pTransform_;
		MeshRenderer* pMeshRenderer_;
		Collider* pCollider_;

		std::stack<DamageObject*> pDamageObjs_;
		// 一秒あたりに回転させる角度
		DictionarySpinBox rotationSpeedSpinBox_;
		float spikeRadius_;
		static unsigned int generateCounter_;
		SpinBox spikeCountSpinBox_;
		static constexpr int MAX_SPIKE_COUNT { 10 };
		bool reverse_;
		SpinBox initialRotationAngleSpinBox_;
	};

} // namespace mtgb