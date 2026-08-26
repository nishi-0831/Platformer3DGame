#pragma once
#include "mtgb.h"

/// <summary>
/// ジャンプを操作する
/// </summary>
class JumpController
{
  public:
	/// <summary>
	/// 操作対象のエンティティIDを受け取る
	/// </summary>
	/// <param name="_targetId"></param>
	JumpController(EntityId _targetId);
	~JumpController();
	/// <summary>
	/// ジャンプの状態を更新
	/// </summary>
	/// <param name="_jumpPressed">ジャンプボタンを押したか否か</param>
	void Update(bool _jumpPressed);
	/// <summary>
	/// ジャンプを開始
	/// 引数の値を最大高度とする
	/// ボタンを押下していた時間によって、高度が減衰する
	/// </summary>
	/// <param name="_maxHeight"></param>
	void StartJump(float _maxHeight);
	/// <summary>
	/// ボタンを離した際に呼ぶ
	/// </summary>
	void ReleaseButton();
	/// <summary>
	/// ジャンプが可能か否か
	/// </summary>
	/// <returns></returns>
	bool CanJump() const;
	/// <summary>
	/// ジャンプバッファの残り時間を返す。
	/// 接地していなくても、着地と同時にジャンプする猶予時間
	/// </summary>
	/// <returns></returns>
	float GetJumpBufferRemainTime() const;

  private:
	Transform* pTargetTransform_;
	RigidBody* pTargetRigidBody_;
	bool isHolding_;
	float gravity_;
	// 低いジャンプを行う際に適用する、重力の倍率
	float lowJumpGravityMultiplier_;
	// これ以上の速度であれば上昇中である、とみなす閾値
	float minAscentVelocityThreshold_;
	// ジャンプボタンの押下をバッファリングする時間
	static constexpr float JUMP_BUFFER_TIME = 0.15f;
	// 接地していなくても、着地と同時にジャンプする猶予時間
	float jumpBufferTimer_;
};