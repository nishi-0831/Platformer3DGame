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
		int audioSourceHandle_;
		// IActor を介して継承されました
		void OnStomped(IActor* _pOther) override;

		void OnHitSide(IActor* _pOther) override;

		void TakeDamage(int _damage) override;
	};
	/// <summary>
	/// 自身を中心にのこぎりを回転させるゲームオブジェクト
	/// </summary>
	class CircularSaw : public GameObject
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

		// 自身からのこぎりまでの柱

		Transform* pPillarTransform_;
		MeshRenderer* pPillarMeshRenderer_;

		// のこぎり
		Saw* pSaw_;
		// のこぎりとの距離
		float sawOffset_;
		// 一秒あたりにのこぎりを回転させる角度
		float rotateAngleSec_;
		static unsigned int generateCounter_;
	};

} // namespace mtgb