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
	enum class STATE
	{
		PATROL, // 巡回
		CHARGE, // 突進
		WAIT, // その場で待機
		RETURN_TO_PATROL // 巡回地点に戻る
	};
	void Patrol();
	void Charge();
	void Wait();
	void ReturnToPatrol();
	bool Search();
	/// <summary>
	/// プレイヤーに体当たりした際の処理
	/// </summary>
	void OnChargePlayer();
	mtstat::MTStat<STATE> state_;
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

	// 突進の速度
	float chargeSpeed_;
	// 突進をする時間。超えると諦めて帰る
	float chargeTime_;
	Vector3 distPos_;
	// 待ち時間
	float waitTime_;
	// 次に遷移する状態
	STATE nextState_;
	// 捜索対象のEntityId
	EntityId targetEntityId_;
	// 巡回地点に戻る速度
	float returnToPatrolSpeed_;
};
