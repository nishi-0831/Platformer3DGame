#include "SphericalCamera.h"

namespace
{
	float normalizedX;
	float normalizedY;
	const mtgb::Vector3 INIT_ANGLE{0, 0, 0};
	/// <summary>
	/// ラジアン単位の値を、0～2πの範囲に正規化する
	/// </summary>
	/// <param name="_angleRad"></param>
	/// <returns></returns>
	float NormalizeAngleRad(float _angleRad)
	{
		const float TWO_PI = DirectX::XM_2PI;

		// 余りを計算
		_angleRad = std::fmod(_angleRad, TWO_PI);

		// 負の場合
		if (_angleRad < 0.0f)
		{
			// 正の値に修正
			_angleRad += TWO_PI;
		}
		return _angleRad;
	}

	mtgb::Mathf::SphericalCoord CameraCartesianToSpherical(const Vector3& _offset)
	{
		float x = _offset.x;
		float y = _offset.y;
		float z = _offset.z;

		float r = sqrtf(x * x + y * y + z * z);

		// カメラの座標系に合わせた変換
		// Y軸が上を向いており、-Y方向にcosθが向いているため
		float theta = std::acosf(-y / r); // カメラ用の極角
		float phi	= std::atan2f(-z, x); // カメラ用の方位角

		return Mathf::SphericalCoord{.r = r, .theta = theta, .phi = phi};
	}
} // namespace

float EaseOutCirc(float _x)
{
	return std::sqrtf(1.0f - std::powf(_x - 1.0f, 2.0f));
}

mtgb::SphericalCamera::SphericalCamera(GameObject* _pGameObj)
	: GameObject(GameObjectBuilder().SetPosition({0, 0, 0}).SetName("Camera").Build())
	, pCameraTransform_{Component<Transform>()}
	, pTargetTransform_{&Transform::Get(_pGameObj->GetEntityId())}
	, inputType_{InputType::JOYPAD}
	, polarAngleRad_{DirectX::XMConvertToRadians(45.0f + 90.0f)}
	, azimuthalAngleRad_{DirectX::XMConvertToRadians(INIT_ANGLE.y + 90.0f)}
	, minPolarAngleRad_{DirectX::XMConvertToRadians(1.0f + 90.0f)}
	, maxPolarAngleRad_{DirectX::XMConvertToRadians(89.0f + 90.0f)}
	, minAzimuthalAngleRad_{DirectX::XMConvertToRadians(1.0f + 90.0f)}
	, maxAzimuthalAngleRad_{DirectX::XMConvertToRadians(359.0f + 90.0f)}
	, lookAtPositionOffset_{0, 1, 0}
	, orbitSpeed_{1.0f}
	, distance_{10.0f}
	, isGrounded_{true}
	, targetVelocityCache_{Vector3::Zero()}
	, lookAtPosLerpProgress_{0.0f}
	, baseY_{0.0f}
	, distY_{0.0f}
	, lerpSpeedGrounded_{1.0f}
	, lerpSpeedJumping_{0.3f}
	, lerpSpeedScalar_{2.0f}
	, yDeadZoneMax_{0.7f}
	, yDeadZoneMin_{0.2f}
{
	// カメラ補間速度の初期化
	cameraStat_
		.OnAnyUpdate(
			[this]
			{
				lookAtPosLerpProgress_ = std::clamp(lookAtPosLerpProgress_, 0.0f, 1.0f);
				// 角度の制限
				polarAngleRad_ = std::clamp(polarAngleRad_, minPolarAngleRad_, maxPolarAngleRad_);
			}
		)
		.OnStart(
			CameraState::GROUNDED,
			[this]
			{
				baseY_ = std::lerp(baseY_, distY_, lookAtPosLerpProgress_);

				lookAtPosLerpProgress_ = 0.0f;
			}
		)
		.OnUpdate(
			CameraState::GROUNDED,
			[this]
			{
				distY_ = pTargetTransform_->position.y;

				orbitSpeed_ = 1.0f;

				// 被写体が画面外にある場合は追従速度を上げる
				float lerpSpeed = IsTargetOffDeadZone() ? lerpSpeedGrounded_ * lerpSpeedScalar_ : lerpSpeedGrounded_;
				lookAtPosLerpProgress_ += lerpSpeed * Time::DeltaTimeF();

				// ジャンプ中：速度に基づいて状態を判定
				if (targetVelocityCache_.y > 0.1f)
				{
					cameraStat_.Change(CameraState::JUMPING);
				}
			}
		);

	cameraStat_
		.OnStart(
			CameraState::JUMPING,
			[this]
			{
				distY_ = pTargetTransform_->position.y;
				baseY_ = std::lerp(baseY_, distY_, lookAtPosLerpProgress_);

				lookAtPosLerpProgress_ = 0.0f;
			}
		)
		.OnUpdate(
			CameraState::JUMPING,
			[this]
			{
				if (isGrounded_)
				{
					cameraStat_.Change(CameraState::GROUNDED);
				}
				distY_		= pTargetTransform_->position.y;
				orbitSpeed_ = 0.5f;

				// 被写体が画面外にある場合は追従速度を上げる
				float lerpSpeed = IsTargetOffDeadZone() ? lerpSpeedJumping_ * lerpSpeedScalar_ : lerpSpeedJumping_;
				lookAtPosLerpProgress_ += lerpSpeed * Time::DeltaTimeF();
			}
		);
}

mtgb::SphericalCamera::~SphericalCamera()
{
}

void mtgb::SphericalCamera::Update()
{
	if (pTargetTransform_ == nullptr)
		return;

	Mathf::SphericalCoord sphericalCoord = CameraCartesianToSpherical(pCameraTransform_->position);
	// ImGui表示(デバッグ用)
	MTImGui::Instance().DirectShow(
		[&, sphericalCoord]()
		{
			TypeRegistry::Instance().CallFunc(pCameraTransform_, "Transform");
			TypeRegistry::Instance().CallFunc(&lookAtPositionOffset_, "lookAtPositionOffset_");

			float degX = DirectX::XMConvertToDegrees(polarAngleRad_) - 90.0f;
			float degY = DirectX::XMConvertToDegrees(azimuthalAngleRad_) - 90.0f;
			ImGui::Text("Polar Angle: %.3f deg", degX);
			ImGui::Text("Azimuthal Angle: %.3f deg", degY);
			ImGui::Text("Target Velocity Y: %.3f", targetVelocityCache_.y);
			ImGui::Text("lookAtPosLerpProgress: %.3f", lookAtPosLerpProgress_);
			ImGui::Text("Is Grounded: %s", isGrounded_ ? "true" : "false");
			ImGui::Text("Is Off Screen: %s", IsTargetOffDeadZone() ? "true" : "false");
			TypeRegistry::Instance().CallFunc(&normalizedX, "normalizedX");
			TypeRegistry::Instance().CallFunc(&normalizedY, "normalizedY");
			// TypeRegistry::Instance().CallFunc(&sphericalCoord.phi, "phi");
			// TypeRegistry::Instance().CallFunc(&sphericalCoord.r, "r");
			// TypeRegistry::Instance().CallFunc(&sphericalCoord.theta, "theta");
		},
		"Camera",
		ShowType::INSPECTOR
	);

	DoOrbit();
	cameraStat_.Update();
	MoveCameraSpherical(distance_);
}

void mtgb::SphericalCamera::Draw() const
{
}

void mtgb::SphericalCamera::DoOrbit()
{
	Vector3 movement;

	// デバイスから入力を取得する
	switch (inputType_)
	{
		// マウス
	case InputType::MOUSE :
		movement = InputUtil::GetMouseMove();
		break;

		// ゲームパッド
	case InputType::JOYPAD :
		Vector2F vec2 = InputUtil::GetAxis(StickType::RIGHT);
		movement.x	  = -vec2.x;
		movement.y	  = vec2.y;
		break;
	}

	if (movement.Size() != 0)
	{
		// 角度を変える

		azimuthalAngleRad_ -= movement.x * orbitSpeed_ * Time::DeltaTimeF();
		polarAngleRad_ += movement.y * orbitSpeed_ * Time::DeltaTimeF();

		// 鉛直角度を制限
		polarAngleRad_ = std::clamp(polarAngleRad_, minPolarAngleRad_, maxPolarAngleRad_);

		// 水平角度を0～2πに正規化
		azimuthalAngleRad_ = NormalizeAngleRad(azimuthalAngleRad_);

		// カメラを動かす
		MoveCameraSpherical(distance_);
	}
}

void mtgb::SphericalCamera::SetFollowMode(bool _isGrounded, const Vector3& _targetVelocity)
{
	isGrounded_			 = _isGrounded;
	targetVelocityCache_ = _targetVelocity;
}

bool mtgb::SphericalCamera::IsTargetOffDeadZone() const
{
	if (pTargetTransform_ == nullptr || pCameraTransform_ == nullptr)
		return false;

	Vector3 targetScreenPos =
		Game::System<CameraSystem>().GetWorldToScreenPos(pTargetTransform_->position, WindowContext::FIRST);
	Vector2F screenSize = Game::System<Screen>().GetSizeF();

	normalizedX = targetScreenPos.x / screenSize.x;
	normalizedY = targetScreenPos.y / screenSize.y;

	bool yOutOfRange = (normalizedY < yDeadZoneMin_) || (normalizedY > yDeadZoneMax_);

	return yOutOfRange;
	// return (normalizedY >= 1.0f) || (normalizedY <= 0.0f) || (normalizedX >= 1.0f) || (normalizedX <= 0.0f);
}

void mtgb::SphericalCamera::MoveCameraSpherical(float _distance)
{
	if (pTargetTransform_ == nullptr)
		return;

	// 注視点を計算
	float lerpedY = std::lerp(baseY_, distY_, lookAtPosLerpProgress_);
	Vector3 lookAtTarget =
		Vector3(pTargetTransform_->position.x, lerpedY, pTargetTransform_->position.z) + lookAtPositionOffset_;

	// 球面座標で新しいカメラ位置を計算
	float theta = polarAngleRad_;
	float phi	= azimuthalAngleRad_;

	Vector3 offset;
	offset.x = _distance * sinf(theta) * cosf(phi);
	offset.y = -_distance * cosf(theta);
	offset.z = -_distance * sinf(theta) * sinf(phi);

	Vector3 newCameraPos = lookAtTarget + offset;

	pCameraTransform_->position = newCameraPos;

	// カメラの回転を設定
	Vector3 lookDir			  = lookAtTarget - pCameraTransform_->position;
	pCameraTransform_->rotate = Quaternion::LookRotation(lookDir.Normalize(), Vector3::Up());
}

Vector3 mtgb::SphericalCamera::ApplyDeadZoneConstraints(const Vector3& proposedCameraPos, const Vector3 _lookAtTarget)
	const
{
	Vector3 adjustedPos = proposedCameraPos;

	// 提案されたカメラ位置での被写体のスクリーン座標を計算
	Vector3 testScreenPos =
		Game::System<CameraSystem>().GetWorldToScreenPos(pTargetTransform_->position, WindowContext::First);
	Vector2F screenSize = Game::System<Screen>().GetSizeF();

	float testNormalizedY = testScreenPos.y / screenSize.y;

	// Y座標がデッドゾーンを超えている場合、カメラの高さを調整
	if (testNormalizedY > yDeadZoneMax_)
	{
		// カメラを下げる
		adjustedPos.y -= (testNormalizedY - yDeadZoneMax_) * screenSize.y * 0.1f; // 調整係数
	}
	else if (testNormalizedY < yDeadZoneMin_)
	{
		// カメラを上げる
		adjustedPos.y += (yDeadZoneMin_ - testNormalizedY) * screenSize.y * 0.1f; // 調整係数
	}

	return adjustedPos;
}
