#include "ICamera.h"
#include "Input/InputData.h"
#include "Core/Time/GameTime.h"
#include "Components/Transform/Transform.h"
mtgb::ICamera::ICamera()
	: polarAngleRad_ { 0.0f }
	, azimuthalAngleRad_ { 0.0f }
	, rotateSensitivity_ { 1.0f }
	, orbitSpeed_ { 1.0f }
	, pCameraTransform_ { nullptr }
	, inputType_ { InputType::MOUSE }
	, distance_ { 5.0f }
	, followTarget_ { false }
	, adjustTargetDirection_ { false } // デフォルトの角度制限（ラジアン）
	, minPolarAngleRad_ { DirectX::XMConvertToRadians(0.1f) }
	, maxPolarAngleRad_ { DirectX::XMConvertToRadians(179.0f) }
	, minAzimuthalAngleRad_ { -(std::numeric_limits<float>::max)() }
	, maxAzimuthalAngleRad_ { (std::numeric_limits<float>::max)() }
	, lookAtPositionOffset_ { 0, 0, 0 }
{
}
void mtgb::ICamera::MoveCameraSpherical(float _distance)
{
	// ref:https://ja.wikipedia.org/wiki/%E7%90%83%E9%9D%A2%E5%BA%A7%E6%A8%99%E7%B3%BB

	// 現在のカメラ位置から回転中心を計算
	Vector3 center = Vector3::Zero();
	// ターゲットを中心に回転
	if (hasTarget_)
	{
		center = pivotPos_ + lookAtPositionOffset_;
	}
	// ターゲットがいない場合は正面を向いて回転
	else
	{
		center = pCameraTransform_->position + (pCameraTransform_->Forward() * _distance) + lookAtPositionOffset_;
	}

	// θ (polar angle) : 鉛直方向
	float theta = polarAngleRad_;

	// φ (azimuthal angle): 水平方向
	float phi = azimuthalAngleRad_;

	// 回転中心からのオフセット
	Vector3 offset;

	// 回転中心の方向を向く

	// 変換
	offset.x = sinf(theta) * cos(phi);
	offset.y = cos(theta);
	offset.z = sin(theta) * sin(phi);

	pCameraTransform_->rotate = Quaternion::LookRotation(offset, Vector3::Up());
	pCameraTransform_->Compute();
	// 位置を反映
	pCameraTransform_->position = center + pCameraTransform_->Back() * _distance;
	pCameraTransform_->Compute();
}

void mtgb::ICamera::DoOrbit()
{
	Vector3 movement;
	switch (inputType_)
	{
		case InputType::MOUSE :
			movement = InputUtil::GetMouseMove();
			break;
		case InputType::JOYPAD :
			Vector2F vec2 = InputUtil::GetAxis(StickType::RIGHT);
			movement.x	  = -vec2.x;
			movement.y	  = vec2.y;
			break;
	}

	if (movement.Size() != 0)
	{
		azimuthalAngleRad_ -= movement.x * orbitSpeed_ * Time::DeltaTimeF();
		polarAngleRad_ += movement.y * orbitSpeed_ * Time::DeltaTimeF();

		// 鉛直角度を制限
		polarAngleRad_ = std::clamp(polarAngleRad_, minPolarAngleRad_, maxPolarAngleRad_);

		float distance = distance_;
		if (hasTarget_)
		{
			distance = (pivotPos_ - pCameraTransform_->position).Size();
		}
		MoveCameraSpherical(distance);
	}
}