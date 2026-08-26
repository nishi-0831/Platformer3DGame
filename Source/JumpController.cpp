#include "stdafx.h"
#include "JumpController.h"

JumpController::JumpController(EntityId _targetId)
	: pTargetTransform_ { &Transform::Get(_targetId) }
	, pTargetRigidBody_ { &RigidBody::Get(_targetId) }
	, isHolding_ { false }
	, lowJumpGravityMultiplier_ { 5.0f }
	, minAscentVelocityThreshold_ { 0.1f }
{
	pTargetRigidBody_->useGravity_ = false;
	gravity_					   = RigidBody::GetGravity();
}

JumpController::~JumpController() {}

void JumpController::Update(bool _jumpPressed)
{
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

	Vector3& velocity = pTargetRigidBody_->velocity_;
	float g			  = gravity_;
	if (isHolding_ == false && velocity.y > minAscentVelocityThreshold_)
	{
		g *= lowJumpGravityMultiplier_;
	}
	velocity += Vector3::Up() * g * Time::DeltaTimeF();
}

void JumpController::StartJump(float _maxHeight)
{
	jumpBufferTimer_			   = 0.0f;
	isHolding_					   = true;
	pTargetRigidBody_->velocity_.y = std::sqrt(2.0f * std::abs(gravity_) * _maxHeight);
}

void JumpController::ReleaseButton()
{
	isHolding_ = false;
}

float JumpController::GetJumpBufferRemainTime() const
{
	return jumpBufferTimer_;
}

bool JumpController::CanJump() const
{
	return jumpBufferTimer_ > 0.0f;
}