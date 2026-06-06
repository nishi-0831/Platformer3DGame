#include "stdafx.h"
#include "QuaternionCamera.h"
#include "ProfileUtlity.h"

mtgb::QuaternionCamera::QuaternionCamera(EntityId _entityId)
	: GameObject(GameObjectBuilder().SetPosition({ 0, 0, 0 }).SetName("Camera").Build())
	, pTransform_ { Component<Transform>() }
	, pTargetTransform_ { &Transform::Get(_entityId) }
	, pTargetRigidBody_ { &RigidBody::Get(_entityId) }
	, lookAtPositionOffset_ { Vector3 { 0, 2, 0 } }
	, rotationSpeedDegPerSec_ { 60.0f }
	, distance_ { 8.0f }
	, inputType_ { InputType::MOUSE }
	, minPitchAngleDeg_ { -5.0f }
	, maxPitchAngleDeg_ { 80.0f }
	, currentLerpSpeed_ { 0.01f }
	, lerpSpeedOnJumping_ { 0.1f }
	, lerpSpeedOnDescending_ { 0.1f }
	, lerpSpeedOnGrounded_ { 0.3f }
{
	rotationSpeedDegPerSec_ = ProfileInt::Load().Section("GAME").Param("CameraSpeed").InitValue(60).Get();
}

mtgb::QuaternionCamera::~QuaternionCamera() 
{
	ProfileInt::Load().Section("Game").Param("CameraSpeed").Write(static_cast<int>(rotationSpeedDegPerSec_));
}

void mtgb::QuaternionCamera::Update()
{
	UpdateLerpSpeed();
	Vector3 movement;

	// デバイスから入力を取得する
	switch (inputType_)
	{
			// マウス
		case InputType::MOUSE :
		{
			movement   = InputUtil::GetMouseAxis();
			float tmp  = movement.x;
			movement.x = movement.y;
			movement.y = tmp;
			break;
		}
			// ゲームパッド
		case InputType::JOYPAD :
		{
			Vector2F vec2 = InputUtil::GetAxis(StickType::RIGHT);
			movement.x	  = vec2.y;
			movement.y	  = vec2.x;
			break;
		}
	}
	Quaternion& rotate		= pTransform_->rotate;
	Quaternion worldRotateY = Quaternion::AngleAxis(
		DirectX::XMConvertToRadians(rotationSpeedDegPerSec_ * Time::DeltaTimeF() * movement.y),
		Vector3::Up()
	);
	// カメラのローカル座標系でのRight軸
	Vector3 localRight = DirectX::XMVector3Rotate(Vector3::Right(), rotate.v);

	// カメラの右方向を軸に回転
	Quaternion localRotateX = Quaternion::AngleAxis(
		DirectX::XMConvertToRadians(rotationSpeedDegPerSec_ * Time::DeltaTimeF() * movement.x),
		localRight
	);

	// 回転を計算
	Quaternion testRotate = rotate * worldRotateY * localRotateX;
	Vector3 testForward	  = DirectX::XMVector3Rotate(Vector3::Forward(), testRotate.v);

	// 制限角度
	float testPitch	  = asinf(-testForward.y);
	float maxPitchRad = DirectX::XMConvertToRadians(maxPitchAngleDeg_);
	float minPitchRad = DirectX::XMConvertToRadians(minPitchAngleDeg_);

	// ピッチ(DirectXでのX軸)角度が制限範囲内なら回転を適用
	if (testPitch <= maxPitchRad && testPitch >= minPitchRad)
	{
		rotate = testRotate;
	}
	else
	{
		// 制限を超えているので、ヨー(DirectXでのY軸)回転のみ適用
		rotate *= worldRotateY;
	}

	lookAtPos_.x = pTargetTransform_->position.x;
	lookAtPos_.y = Mathf::Lerp(lookAtPos_.y, pTargetTransform_->position.y, currentLerpSpeed_);
	lookAtPos_.z = pTargetTransform_->position.z;

	Vector3 toTargetDir	  = DirectX::XMVector3Rotate(Vector3::Forward(), rotate.v);
	pTransform_->position = lookAtPos_ - (toTargetDir * distance_) + lookAtPositionOffset_;

	MTImGui::DirectShow(
		[this]()
		{
			ImGui::InputFloat("s", &currentLerpSpeed_);
			PropertyDisplayRegistry::Instance().ShowProperty(&pTargetRigidBody_->velocity_, "vel");
		},
		"lerpSpeed",
		ShowType::INSPECTOR
	);
}

void mtgb::QuaternionCamera::UpdateLerpSpeed()
{
	if (pTargetRigidBody_->isGround_)
	{
		currentLerpSpeed_ = lerpSpeedOnGrounded_;
		return;
	}
	Vector3 velocity = pTargetRigidBody_->velocity_;

	if (velocity.y > 0.0f)
	{
		currentLerpSpeed_ = lerpSpeedOnJumping_;
	}
	else
	{
		currentLerpSpeed_ = lerpSpeedOnDescending_;
	}
}
