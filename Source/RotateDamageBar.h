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
		/// <summary>
		/// ダメージオブジェクトを一つ作成
		/// </summary>
		void AddDamageObject();
		/// <summary>
		/// ダメージオブジェクトを一つ削除
		/// </summary>
		void RemoveDamageObject();
		nlohmann::json SerializeProperties() const override;
		void DeserializeProperties(const nlohmann::json& _json) override;

	  private:
		/// <summary>
		/// 初期値だけ、回転する。プレイシーン開始時に呼ぶ用
		/// </summary>
		void RotateInitialAngle();
		Transform* pTransform_;
		MeshRenderer* pMeshRenderer_;
		Collider* pCollider_;

		std::stack<DamageObject*> pDamageObjs_;
		// 一秒あたりに回転させる角度
		DictionarySpinBox rotationSpeedSpinBox_;
		// ダメージオブジェクトの半径。コライダーが球であること前提
		float damageObjRadius_;
		static unsigned int generateCounter_;
		// ダメージオブジェクトの個数のスピンボックス
		SpinBox damageObjCountSpinBox_;
		static constexpr int MAX_DAMAGE_OBJ_COUNT { 10 };
		// 回転が負方向か否か
		bool reverse_;
		// プレイシーン開始時の初期回転角度
		SpinBox initialRotationAngleSpinBox_;
	};

} // namespace mtgb