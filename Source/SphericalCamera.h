#pragma once
#include <mtgb.h>

namespace mtgb
{
	class SphericalCamera : public GameObject
	{
	  public:
		// Camera();
		SphericalCamera(GameObject* _pGameObj);
		~SphericalCamera();

		void Update() override;
		void Draw() const override;

		/// <summary>
		/// 被写体の周りを円を描くように回転させる
		/// </summary>
		void DoOrbit();
		/// <summary>
		/// <para> 追従のモードを設定する </para>
		/// <para> 被写体がカメラに自身の情報を渡す </para>
		/// </summary>
		/// <param name="_isGrounded"> 被写体が接地しているか </param>
		/// <param name="_targetVelocity"> 被写体の速度 </param>
		void SetFollowMode(bool _isGrounded, const Vector3& _targetVelocity);

	  private:
		bool IsTargetOffDeadZone() const;
		/// <summary>
		/// 球面座標系でカメラを動かす
		/// 視点を中心に回転させる
		/// </summary>
		/// <param name="_distance">視点との距離</param>
		void MoveCameraSpherical(float _distance);

		Vector3 ApplyDeadZoneConstraints(const Vector3& _proposedCameraPos, const Vector3 _lookAtTarget) const;
		enum class CameraState
		{
			GROUNDED, // 着地状態：通常のカメラ追従
			JUMPING,
			JUMP_ASCENDING, // ジャンプ上昇中：角度を浅くしていく
			JUMP_APEX,		// ジャンプ頂点：最小角度を維持
			JUMP_DESCENDING // ジャンプ下降中：角度を回復していく
		};
		// カメラの状態
		mtstat::MTStat<CameraState> cameraStat_;

		// カメラのトランスフォーム
		Transform* pCameraTransform_;
		// 被写体のトランスフォーム
		Transform* pTargetTransform_;
		// 入力デバイスの種類
		InputType inputType_;
		// 垂直角度
		float polarAngleRad_;
		// 水平角度
		float azimuthalAngleRad_;
		// -Y側、下側が0度、+Y側、上側が90度
		float minPolarAngleRad_, maxPolarAngleRad_;
		// +X側、右側が0度、-X側、左側が90度
		float minAzimuthalAngleRad_, maxAzimuthalAngleRad_;
		// 視点のオフセット
		Vector3 lookAtPositionOffset_;
		// カメラを回転させる速度
		float orbitSpeed_;
		// 視点との距離
		float distance_;
		// 被写体が接地しているか
		bool isGrounded_;
		// 被写体の速度のキャッシュ
		Vector3 targetVelocityCache_;
		// 視点を補間するための進行度
		float lookAtPosLerpProgress_;
		// 補間の始点
		float baseY_;
		// 補間の終点
		float distY_;
		float lerpSpeedGrounded_;
		float lerpSpeedJumping_;
		float lerpSpeedScalar_;
		// デッドゾーン設定
		float yDeadZoneMax_;
		float yDeadZoneMin_;
	};
} // namespace mtgb
