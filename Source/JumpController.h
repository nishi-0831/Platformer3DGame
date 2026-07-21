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

	void Update();
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

  private:
	Transform* pTargetTransform_;
	RigidBody* pTargetRigidBody_;
	bool isHolding_;
	float gravity_;
	// 低いジャンプを行う際に適用する、重力の倍率
	float lowJumpGravityMultiplier_;
	// これ以上の速度であれば上昇中である、とみなす閾値
	float minAscentVelocityThreshold_;
};
