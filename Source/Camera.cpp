#include "Camera.h"

namespace
{
	const mtgb::Vector3 INIT_ANGLE{ 0, 0, 0 };
}

float EaseOutCirc(float x)
{
	return std::sqrtf(1.0f - std::powf(x - 1.0f, 2.0f));
}

mtgb::Camera::Camera()
{
	pCameraTransform_ = Component<Transform>();
}

mtgb::Camera::Camera(GameObject* _pGameObj) : GameObject(GameObjectBuilder()
	.SetPosition({ 0, 0, 0 })
	.SetName("Camera")
	.Build())
	, isGrounded_{true}
	, inputType_{ InputType::JOYPAD }
	, polarAngleRad_{ DirectX::XMConvertToRadians(45.0f + 90.0f) }
	, azimuthalAngleRad_{ DirectX::XMConvertToRadians(INIT_ANGLE.y + 90.0f) }
	, orbitSpeed_{1.0f}
	, distance_{10.0f}
	, minPolarAngleRad_{ DirectX::XMConvertToRadians(1.0f + 90.0f) }
	, maxPolarAngleRad_{ DirectX::XMConvertToRadians(89.0f + 90.0f) }
	, lookAtPositionOffset_{ 0, 1, 0 }
	, pCameraTransform_{Component<Transform>()}
	, pTargetTransform_{&Transform::Get(_pGameObj->GetEntityId())}
	, targetVelocityCache_{ Vector3::Zero()}
	, baseY{0.0f}
	, distY{0.0f}
	, lookAtPosLerpProgress{0.0f}
{
	// カメラ補間速度の初期化
	cameraStat_
		.OnAnyUpdate([this]
			{
				lookAtPosLerpProgress = std::clamp(lookAtPosLerpProgress, 0.0f, 1.0f);
				// 角度の制限
				polarAngleRad_ = std::clamp(polarAngleRad_, minPolarAngleRad_, maxPolarAngleRad_);
			})
		.OnStart(CameraState::GROUNDED, [this]
			{
				baseY = std::lerp(baseY, distY, lookAtPosLerpProgress);

				lookAtPosLerpProgress = 0.0f;
			})
		.OnUpdate(CameraState::GROUNDED, [this]
			{
				distY = pTargetTransform_->position.y;


				orbitSpeed_ = 1.0f;
				lookAtPosLerpProgress += 0.5f * Time::DeltaTimeF();

				// ジャンプ中：速度に基づいて状態を判定
				if (targetVelocityCache_.y > 0.1f)
				{
					cameraStat_.Change(CameraState::JUMPING);
				}
			});

	cameraStat_.OnStart(CameraState::JUMPING, [this]
		{
			distY = pTargetTransform_->position.y;
			baseY = std::lerp(baseY, distY, lookAtPosLerpProgress);

			lookAtPosLerpProgress = 0.0f;
		}).OnUpdate(CameraState::JUMPING, [this]
			{
				if (isGrounded_)
				{
					cameraStat_.Change(CameraState::GROUNDED);
				}
				distY = pTargetTransform_->position.y;
				orbitSpeed_ = 0.5f;
				// 目標角度へ滑らかに補間
				
				lookAtPosLerpProgress += 0.3f * Time::DeltaTimeF();
			});
}

mtgb::Camera::~Camera()
{
}

void mtgb::Camera::Update()
{
	if (pTargetTransform_ == nullptr)
		return;


	// ImGui表示（デバッグ用）
	MTImGui::Instance().DirectShow([&]()
	{
		TypeRegistry::Instance().CallFunc<Transform>(pCameraTransform_, "Transform");
		TypeRegistry::Instance().CallFunc<Vector3>(&lookAtPositionOffset_, "lookAtPositionOffset_");

		float degX = DirectX::XMConvertToDegrees(polarAngleRad_) - 90.0f;
		float degY = DirectX::XMConvertToDegrees(azimuthalAngleRad_) - 90.0f;
		ImGui::Text("Polar Angle: %.3f deg", degX);
		ImGui::Text("Azimuthal Angle: %.3f deg", degY);
		ImGui::Text("Target Velocity Y: %.3f", targetVelocityCache_.y);
		ImGui::Text("lookAtPosLerpProgress: %.3f", lookAtPosLerpProgress);
		ImGui::Text("Is Grounded: %s", isGrounded_ ? "true" : "false");
	}, "Camera", ShowType::Inspector);

	DoOrbit();
	//UpdateCameraState();
	cameraStat_.Update();
	MoveCameraSpherical(distance_);
}

void mtgb::Camera::Draw() const
{
}

void mtgb::Camera::DoOrbit()
{
	Vector3 movement;
	switch (inputType_)
	{
	case InputType::MOUSE:
		movement = InputUtil::GetMouseMove();
		break;
	case InputType::JOYPAD:
		Vector2F vec2 = InputUtil::GetAxis(StickType::RIGHT);
		movement.x = -vec2.x;
		movement.y = vec2.y;
		break;
	}

	if (movement.Size() != 0)
	{
		azimuthalAngleRad_ -= movement.x * orbitSpeed_ * Time::DeltaTimeF();
		polarAngleRad_ += movement.y * orbitSpeed_ * Time::DeltaTimeF();

		// 鉛直角度を制限
		polarAngleRad_ = std::clamp(polarAngleRad_, minPolarAngleRad_, maxPolarAngleRad_);
		azimuthalAngleRad_ = std::clamp(azimuthalAngleRad_, minAzimuthalAngleRad_, maxAzimuthalAngleRad_);

		MoveCameraSpherical(distance_);
	}
}

void mtgb::Camera::SetFollowMode(bool _isGrounded, const Vector3& _targetVelocity)
{
	isGrounded_ = _isGrounded;
	targetVelocityCache_ = _targetVelocity;
}


void mtgb::Camera::UpdateCameraState()
{
}

void mtgb::Camera::MoveCameraSpherical(float _distance)
{
	if (pTargetTransform_ == nullptr)
		return;

	// 注視点を計算
	float lerpedY = std::lerp(baseY, distY,lookAtPosLerpProgress);
	Vector3 lookAtTarget = Vector3(pTargetTransform_->position.x, lerpedY,pTargetTransform_->position.z) + lookAtPositionOffset_;

	// 球面座標で新しいカメラ位置を計算
	float theta = polarAngleRad_;
	float phi = azimuthalAngleRad_;

	Vector3 offset;
	offset.x = _distance * sinf(theta) * cosf(phi);
	offset.y = -_distance * cosf(theta);
	offset.z = -_distance * sinf(theta) * sinf(phi);

	Vector3 newCameraPos = lookAtTarget + offset;

	pCameraTransform_->position = newCameraPos;
	
	// カメラの回転を設定
	Vector3 lookDir = lookAtTarget - pCameraTransform_->position;
	pCameraTransform_->rotate = Quaternion::LookRotation(lookDir.Normalize(), Vector3::Up());
}
