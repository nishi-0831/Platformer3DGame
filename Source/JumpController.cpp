#include "stdafx.h"
#include "JumpController.h"

JumpController::JumpController(EntityId _targetId)
	: pTargetTransform_{&Transform::Get(_targetId)}
	, pTargetRigidBody_{&RigidBody::Get(_targetId)}
	, isJumping_{false}
	, isHolding_{false}
	, holdTime_{0.0f}
	, onStartJumpVelocity_{0.0f}
	, maxHoldTime_{0.5f}
{
}

JumpController::~JumpController()
{
}

void JumpController::Update()
{
	Vector3& velocity = pTargetRigidBody_->velocity_;
	if (velocity.y > 0.0f)
	{
		isJumping_ = true;
	}
	if (isHolding_ && isJumping_)
	{
		holdTime_ += Time::DeltaTimeF();
	}
}

void JumpController::StartJump(float _maxHeight)
{
	isJumping_ = true;
	isHolding_ = true;

	float gravity	  = RigidBody::GetGravity();
	Vector3& velocity = pTargetRigidBody_->velocity_;
	onStartJumpVelocity_		  = std::sqrt(2.0f * std::abs(gravity) * _maxHeight);
	velocity.y			 = onStartJumpVelocity_;
}

void JumpController::ReleaseButton()
{
	isHolding_ = false;
	// ボタンを押していた時間によって、高度を減衰させる

	float holdFraction = std::clamp(holdTime_ / maxHoldTime_,0.0f,1.0f);
	float gravity	   = RigidBody::GetGravity();
	float maxHeight	   = (onStartJumpVelocity_ * onStartJumpVelocity_) / (2.0f * gravity);

	float currHeight = maxHeight * holdFraction;
	//float 
}
