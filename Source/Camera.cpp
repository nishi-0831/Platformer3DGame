#include "Camera.h"

namespace
{
	const mtgb::Vector3 INIT_ANGLE{ 0, 0, 0 };
	float normalizedX;
	float normalizedY;
	bool isTargetInDeadZone;
	float easedProgress;
	float groundedPolarAngleRad{ 0.0f };
	float jumpOrbitSpeed{0.0f};
	float groundedOrbitSpeed{ 0.0f };

	float lastGroundCameraY;
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
	, wasGrounded_{true}
	, groundedPolarAngleDeg_{45.0f}
	, jumpAscendingPolarAngleDeg_{15.0f}
	, deadZoneScreenX_{0.4f}
	, deadZoneScreenYMin_{0.3f}
	, deadZoneScreenYMax_{0.7f}
	, useDeadZone_{true}
	, angleLerpProgress_{0.0f}
	, lerpedPolarAngleStart_{0.0f}
{
	inputType_ = InputType::JOYPAD;
	polarAngleRad_ = DirectX::XMConvertToRadians(groundedPolarAngleDeg_ + 90.0f);
	azimuthalAngleRad_ = DirectX::XMConvertToRadians(INIT_ANGLE.y + 90.0f);

	orbitSpeed_ = 1.0f;
	distance_ = 10.0f;
	followTarget_ = true;
	adjustTargetDirection_ = false;
	minPolarAngleRad_ = DirectX::XMConvertToRadians(1.0f + 90.0f);
	maxPolarAngleRad_ = DirectX::XMConvertToRadians(89.0f + 90.0f);
	lookAtPositionOffset_ = { 0, 1, 0 };

	pCameraTransform_ = Component<Transform>();
	pTargetTransform_ = &Transform::Get(_pGameObj->GetEntityId());
	targetVelocityCache_ = Vector3::Zero();
	lastGroundCameraY = pCameraTransform_->position.y;

	// カメラ補間速度の初期化
	targetCameraLerpSpeed_ = 0.1f;
	targetAngleLerpSpeed_ = 0.1f;

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
				targetCameraLerpSpeed_ = 0.5f;
				targetAngleLerpSpeed_ = 0.3f * Time::DeltaTimeF();
			})
		.OnUpdate(CameraState::GROUNDED, [this]
			{
				distY = pTargetTransform_->position.y;

				lastGroundCameraY = pCameraTransform_->position.y;

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
			groundedPolarAngleRad = polarAngleRad_;
			targetAngleLerpSpeed_ = 0.01f;
		}).OnUpdate(CameraState::JUMPING, [this]
			{
				if (isGrounded_)
				{
					lastGroundedY = pTargetTransform_->position.y;
					lastGroundCameraY = pCameraTransform_->position.y;

					cameraStat_.Change(CameraState::GROUNDED);
				}
				distY = pTargetTransform_->position.y;
				orbitSpeed_ = 0.5f;
				angleLerpProgress_ = 0.0f;
				easedProgress = 0.0f;
				// 目標角度へ補間
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

	isTargetInDeadZone = IsTargetInDeadZone();

	// ImGui表示(デバッグ用)
	MTImGui::Instance().DirectShow([&]()
	{
		TypeRegistry::Instance().CallFunc<Transform>(pCameraTransform_, "Transform");
		TypeRegistry::Instance().CallFunc<float>(&jumpAscendingPolarAngleDeg_, "jumpAscendingPolarAngleDeg");
		TypeRegistry::Instance().CallFunc<float>(&angleLerpProgress_, "angleLerpProgress_");
		TypeRegistry::Instance().CallFunc<float>(&easedProgress, "easedProgress");
		TypeRegistry::Instance().CallFunc<Vector3>(&lookAtPositionOffset_, "lookAtPositionOffset_");

		float degX = DirectX::XMConvertToDegrees(polarAngleRad_) - 90.0f;
		float degY = DirectX::XMConvertToDegrees(azimuthalAngleRad_) - 90.0f;
		ImGui::Text("Polar Angle: %.3f deg", degX);
		ImGui::Text("Azimuthal Angle: %.3f deg", degY);
		ImGui::Text("Target Velocity Y: %.3f", targetVelocityCache_.y);
		ImGui::Text("lookAtPosLerpProgress: %.3f", lookAtPosLerpProgress);
		ImGui::Text("lastGroundedY: %.3f", lastGroundedY);
		ImGui::Text("lastGroundCameraY: %.3f", lastGroundCameraY);
		ImGui::Text("Is Grounded: %s", isGrounded_ ? "true" : "false");
		ImGui::Text("Normalized Screen Pos: (%.3f, %.3f)", normalizedX, normalizedY);
	}, "Camera", ShowType::Inspector);

	DoOrbit();
	//UpdateCameraState();
	cameraStat_.Update();
	MoveCameraSpherical(distance_);
}

void mtgb::Camera::Draw() const
{
}

void mtgb::Camera::SetFollowMode(bool _isGrounded, const Vector3& _targetVelocity)
{
	wasGrounded_ = isGrounded_;
	isGrounded_ = _isGrounded;
	targetVelocityCache_ = _targetVelocity;
}

void mtgb::Camera::SetDeadZone(float _screenPercentageX, float _screenPercentageY)
{
	deadZoneScreenX_ = std::clamp(_screenPercentageX, 0.0f, 1.0f);
	deadZoneScreenYMin_ = std::clamp(_screenPercentageY * 0.25f, 0.0f, 0.5f);
	deadZoneScreenYMax_ = std::clamp(_screenPercentageY * 0.75f, 0.5f, 1.0f);
	useDeadZone_ = true;
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

bool mtgb::Camera::IsTargetInDeadZone() const
{
	if (pTargetTransform_ == nullptr || pCameraTransform_ == nullptr)
		return false;

	Vector3 targetScreenPos = Game::System<CameraSystem>().GetWorldToScreenPos(
		pTargetTransform_->position,
		WindowContext::First
	);
	Vector2F screenSize = Game::System<Screen>().GetSizeF();
	
	normalizedX = targetScreenPos.x / screenSize.x;
	normalizedY = targetScreenPos.y / screenSize.y;

	return (normalizedY >= deadZoneScreenYMin_) && (normalizedY <= deadZoneScreenYMax_);
}
