#pragma once
#include <mtgb.h>

namespace mtgb
{
	class Camera : public GameObject
	{
	public:
		Camera();
		Camera(GameObject* _pGameObj);
		~Camera();

		void Update() override;
		void Draw() const override;

		void DoOrbit();
		void SetFollowMode(bool _isGrounded, const Vector3& _targetVelocity);
		void MoveCameraSpherical(float _distance);
	private:
		enum class CameraState
		{
			GROUNDED,		// 着地状態：通常のカメラ追従
			JUMPING,
			JUMP_ASCENDING,	// ジャンプ上昇中：角度を浅くしていく
			JUMP_APEX,		// ジャンプ頂点：最小角度を維持
			JUMP_DESCENDING	// ジャンプ下降中：角度を回復していく
		};
		mtstat::MTStat<CameraState> cameraStat_;

		Transform* pCameraTransform_;
		Transform* pTargetTransform_;
		InputType inputType_;
		float polarAngleRad_;
		float azimuthalAngleRad_;
		// -Y側、下側が0度、+Y側、上側が90度
		float minPolarAngleRad_, maxPolarAngleRad_;
		// +X側、右側が0度、-X側、左側が90度
		float minAzimuthalAngleRad_, maxAzimuthalAngleRad_;
		Vector3 lookAtPositionOffset_;
		float orbitSpeed_;
		float distance_;
		// ジャンプ状態管理
		bool isGrounded_;
		Vector3 targetVelocityCache_;
		float lookAtPosLerpProgress;
		float baseY;
		float distY;

		// ヘルパー関数
		void UpdateCameraState();
	};
}

