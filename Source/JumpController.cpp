#include "stdafx.h"
#include "JumpController.h"

JumpController::JumpController(EntityId _targetId)
	: pTargetTransform_ { &Transform::Get(_targetId) }
	, pTargetRigidBody_ { &RigidBody::Get(_targetId) }
	, isHolding_ { false }
	, lowJumpGravityMultiplier_ { 5.0f }
	, minAscentVelocityThreshold_ { 0.1f }
	, jumpBufferTimer_ { 0.0f }
	, coyoteTimer_ { 0.0f }
	, jumpVelocityY_ { 0.0f }
{
	pTargetRigidBody_->useGravity_ = false;
	gravity_					   = RigidBody::GetGravity();
}

JumpController::~JumpController() {}

void JumpController::Update(bool _jumpPressed)
{
	if (pTargetRigidBody_->isGround_)
	{
		coyoteTimer_ = COYOTE_TIME;

		// 接地中は落下速度が残らないようにする
		if (jumpVelocityY_ < 0.0f)
		{
			jumpVelocityY_ = 0.0f;
		}
	}
	else
	{
		coyoteTimer_ -= Time::DeltaTimeF();
		if (coyoteTimer_ < 0.0f)
		{
			coyoteTimer_ = 0.0f;
		}
	}

	if (_jumpPressed)
	{
		jumpBufferTimer_ = JUMP_BUFFER_TIME;
	}
	else
	{
		jumpBufferTimer_ -= Time::DeltaTimeF();
		if (jumpBufferTimer_ < 0.0f)
		{
			jumpBufferTimer_ = 0.0f;
		}
	}

	float gravity = gravity_;
	if (isHolding_ == false && jumpVelocityY_ > minAscentVelocityThreshold_)
	{
		gravity *= lowJumpGravityMultiplier_;
	}
	jumpVelocityY_ += gravity * Time::DeltaTimeF();
}

void JumpController::StartJump(float _maxHeight)
{
	jumpBufferTimer_ = 0.0f;
	isHolding_		 = true;
	jumpVelocityY_	 = std::sqrt(2.0f * std::abs(gravity_) * _maxHeight);
}

void JumpController::ReleaseButton()
{
	isHolding_ = false;
}

float JumpController::GetJumpBufferRemainTime() const
{
	return jumpBufferTimer_;
}

bool JumpController::IsFalling() const
{
	return jumpVelocityY_ < 0.0f && pTargetRigidBody_->isGround_ == false && coyoteTimer_ == 0.0f;
}

float JumpController::GetVelocityY() const
{
	return jumpVelocityY_;
}

bool JumpController::CanJump() const
{
	return jumpBufferTimer_ > 0.0f && coyoteTimer_ > 0.0f;
}