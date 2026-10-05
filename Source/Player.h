#pragma once
#include <mtgb.h>
#include "QuaternionCamera.h"
#include "IActor.h"
#include "HPViewer.h"
#include "JumpController.h"

class Player : public mtgb::GameObject, public IActor
{
  public:
	Player();
	~Player();

	void Update() override;
	void Start() override;
	void ShowImGui() override;
	/// <summary>
	/// 他のアクターから踏まれた際の処理
	/// </summary>
	/// <param name="_pOther"></param>
	void OnStomped(IActor* _pOther) override;
	/// <summary>
	/// 他のアクターが横から衝突した際の処理
	/// </summary>
	/// <param name="_pOther"></param>
	void OnHitSide(IActor* _pOther) override;
	/// <summary>
	/// プレイヤーがダメージを受ける際の処理
	/// </summary>
	/// <param name="_damage"></param>
	void TakeDamage(int _damage) override;
	/// <summary>
	/// プレイヤー自身以外から与えられる速度を追加。
	/// また、瞬間的な速度変化のみ
	/// </summary>
	/// <param name="_velocity"></param>
	void AddExternalVelocity(const Vector3& _velocity) override;
	/// <summary>
	/// プレイヤー自身以外から与えられる速度を追加。
	/// 継続的に与えられるような速度のみ
	/// </summary>
	/// <param name="_velocity"></param>
	void SetSurfaceVelocity(const Vector3& _velocity) override;

  private:
	/// <summary>
	/// 現在の値から目的の値まで、一定の値で進める
	/// </summary>
	/// <param name="_curr">現在値</param>
	/// <param name="_target">目的値</param>
	/// <param name="_maxDelta">移動量</param>
	/// <returns>進めた値</returns>
	Vector3 MoveTowards(const Vector3& _curr, const Vector3& _target, float _maxDelta);
	/// <summary>
	/// 現在の値から目的の値まで、一定の値で進める
	/// </summary>
	/// <param name="_curr">現在値</param>
	/// <param name="_target">目的値</param>
	/// <param name="_maxDelta">移動量</param>
	/// <returns>進めた値</returns>
	float MoveTowards(float _curr, float _target, float _maxDelta);
	/// <summary>
	/// プレイヤーの移動方向を取得
	/// </summary>
	/// <returns></returns>
	Vector3 GetMoveDir();
	/// <summary>
	/// 移動速度を更新
	/// </summary>
	void UpdateVelocity();
	/// <summary>
	/// プレイヤーの向きを更新
	/// </summary>
	void UpdateRotate();
	/// <summary>
	/// 他のオブジェクトと接触した際の処理
	/// </summary>
	/// <param name="_entityId"></param>
	void OnCollisionEnter(EntityId _entityId);
	void InitializeState();
	enum class STATE
	{
		IDLE,
		WALK,
		JUMP,
		FALL,
		DYING,
		VICTORY,
		RUN
	};
	/// <summary>
	/// 移動状態(WALK、RUN)のプレイヤーが次に遷移する状態を取得する
	/// </summary>
	/// <param name="_state">次の状態を格納する変数</param>
	/// <returns>状態遷移が必要な場合はtrue、そうでないならfalse</returns>
	bool TryGetNextStateOnMove(STATE& _state);

	// プレイヤーのアニメーション状態を管理するステートマシン
	mtstat::MTStat<STATE> state_;
	Transform* pTransform_;
	Collider* pCollider_;
	MeshRenderer* pMeshRenderer_;
	RigidBody* pRigidBody_;

	// プレイヤーに追従するカメラ
	QuaternionCamera* pCamera_;
	const Transform* pCameraTransform_;
	std::optional<FbxAnimationController> animController_;
	// ジャンプ処理を管理するクラス
	JumpController jumpController_;
	// プレイヤーのHPを表示するUI
	HPViewer* pHPViewer_;
	// プレイヤーのHP
	int hp_;
	// 無敵かどうか
	bool isInvincible_;
	// 被弾時、無敵になる時間(秒)
	float invincibilityTimeSec_;
	// 無敵時間中の、描画有無を切り替える間隔
	float changeVisibilitySpan_;
	// 無敵になってからの経過時間
	float elapsedInvincibilityTime_;
	// 描画有無を切り替える処理のハンドル
	TimerHandle hTimerChangeVisibility_;

	// 歩いている際に煙のエフェクトを再生する間隔
	float walkSmokeInterval_;
	// 煙のエフェクトを出す間隔を計る経過時間
	float walkSmokeElapsedTime_;
	// 通常ジャンプの高さ
	float walkJumpHeight_;
	// ダッシュジャンプの高さ
	float runJumpHeight_;
	// 歩くスピード
	float walkSpeed_;
	// 走るスピード
	float dashSpeed_;
	// 加速度
	float acceleration_;
	// 走っているか否か
	bool isRunning_;
	// 外部から与えられた速度が減衰する速度
	float externalDeceleration_;
	// 接地中、外部速度が減衰する際の摩擦
	float friction_;
	// プレイヤー自身の移動以外から与えられる瞬間的な速度
	Vector3 externalVelocity_;
	// プレイヤー自身の移動速度
	Vector3 movementVelocity_;
	// ダッシュジャンプ中か否か
	bool isDashJumping_;
	// プレイヤー自身の移動以外から与えられる継続的な速度
	Vector3 surfaceVelocity_;
};