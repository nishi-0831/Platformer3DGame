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
	bool isJumping_;
	bool isHolding_;
	// ボタンを押下していた時間
	float holdTime_;
	// 最大押下時間。この時間以上押下しても高度は高くならない
	float maxHoldTime_;
	float onStartJumpVelocity_;
};
