#pragma once
#include <mtgb.h>

namespace mtgb
{
	class Camera : public GameObject, public ICamera
	{
	public:
		Camera();
		Camera(GameObject* _pGameObj);
		~Camera();

		void Update() override;
		void Draw() const override;

		void SetFollowMode(bool _isGrounded, const Vector3& _targetVelocity);
		void SetDeadZone(float _screenPercentageX, float _screenPercentageY);
		void MoveCameraSpherical(float _distance) override;
		bool IsTargetInDeadZone() const;

	private:
		enum class CameraState
		{
			GROUNDED,		// 着地状態：通常のカメラ追従
			JUMP_ASCENDING,	// ジャンプ上昇中：角度を浅くしていく
			JUMP_APEX,		// ジャンプ頂点：最小角度を維持
			JUMP_DESCENDING	// ジャンプ下降中：角度を回復していく
		};

		// ジャンプ状態管理
		CameraState cameraState_;
		CameraState prevCameraState_;
		float targetPolarAngle_;		// 目標の鉛直角度
		float targetCameraLerpSpeed_;	// カメラ位置の補間速度
		float targetAngleLerpSpeed_;	// 角度変更の補間速度

		// ジャンプパラメータ
		float groundedPolarAngleDeg_;		// 着地時の角度
		float jumpAscendingPolarAngleDeg_;	// ジャンプ上昇時の角度
		float jumpApexPolarAngleDeg_;		// ジャンプ頂点時の角度
		float jumpDescendingPolarAngleDeg_;	// ジャンプ下降時の角度

		float lerpedPolarAngle_;
		float lerpedPolarAngleStart_;
		float angleLerpProgress_;
		bool isGrounded_;
		bool wasGrounded_;
		Vector3 targetVelocityCache_;

		// デッドゾーン関連
		float deadZoneScreenX_;
		float deadZoneScreenYMin_;
		float deadZoneScreenYMax_;
		bool useDeadZone_;

		// ヘルパー関数
		void UpdateCameraState();
		void UpdateCameraAngle(float _deltaTime);
		void UpdateCameraPosition(float _deltaTime);
	};
}

