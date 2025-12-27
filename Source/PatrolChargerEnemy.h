#pragma once
#include <mtgb.h>
class PatrolChargerEnemy : public mtgb::GameObject, mtgb::ImGuiShowable
{
public:
	PatrolChargerEnemy();
	~PatrolChargerEnemy();
	void Update() override;
	void Draw() const override;
	void Start() override;
	void ShowImGui() override;
private:
	/// <summary>
	/// プレイヤーに体当たりした際の処理
	/// </summary>
	void OnChargePlayer();
	Transform* pTransform_;
	RigidBody* pRigidBody_;
	MeshRenderer* pMeshRenderer_;
	Collider* pCollider_;
	
	// 捜索対象のTransform
	Transform* pTargetTransform_;
	// 発見と判定する角度
	float foundFOV_;
	// 発見と判定する距離
	float foundDistance_;
	// 巡回に使う
	Interpolator* pInterpolator_;
	static unsigned int generateCounter_;
	// 捜索対象のEntityId
	EntityId targetEntityId_;
	// 巡回地点に戻る速度
	float returnToPatrolSpeed_;
	std::optional<FbxAnimationController> animController_;
private:
	// 状態
	enum class STATE
	{
		PATROL, // 巡回
		CHARGE, // 突進
		WAIT, // その場で待機
		RETURN_TO_PATROL, // 巡回地点に戻る
		DYING // 死亡状態
	};
	void InitializeState();
	void Patrol();
	void Charge();
	void Wait();
	void ReturnToPatrol();
	void Dying();
	bool Search();
	mtstat::MTStat<STATE> state_;

	// ------巡回状態------
	// ターゲットを見つけてから突進状態に遷移するまでの待機時間
	float waitTimeTransitionCharge_;
	// -------------------

	// ------突進状態------
	// 突進の速度
	float chargeSpeed_;
	// 突進をする時間。超えると諦めて帰る
	float chargeTime_;
	// 衝突後の待ち時間
	float waitTimeAfterCharge_;
	// --------------------
	
	// ------歩き状態-----
	// 歩くアニメーションの再生速度
	float walkAnimSpeed_;
	// -------------------
	
	// ------待機状態------
	// 待ち時間
	float waitTime_;
	// 次に遷移する状態
	STATE nextState_;
	// -------------------
};
