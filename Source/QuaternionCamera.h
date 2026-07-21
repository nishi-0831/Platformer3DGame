#pragma once
#include "mtgb.h"

namespace mtgb
{
	class QuaternionCamera : public GameObject
	{
	  public:
		QuaternionCamera(EntityId _entityId);
		~QuaternionCamera();
		void Update() override;

	  private:
		Transform* pTransform_;
		Transform* pTargetTransform_;
		RigidBody* pTargetRigidBody_;

		// 被写体の座標
		Vector3 lookAtPos_;
		// 被写体の座標とのオフセット。被写体を中心からちょっとずらす
		Vector3 lookAtPositionOffset_;
		// 一秒あたりの回転速度(度)
		float rotationSpeedDegPerSec_;
		// 被写体との距離
		float distance_;
		// カメラを動かす入力機器の種類
		InputType inputType_;
		// 上下の角度の最小値
		float minPitchAngleDeg_;
		// 上下の角度の最大値
		float maxPitchAngleDeg_;
		// 進行度がイージングされた値。カメラの補間に使う
		float cameraEasedProgress_;
		// 上昇時のカメラ補間速度
		float lerpSpeedOnJumping_;

		// 落下時のカメラ補間速度
		float lerpSpeedOnDescending_;
		// 接地時のカメラ補間速度
		float lerpSpeedOnGrounded_;
		// 補間の進行度
		float lerpProgress_;
		// 補間の目的値(y座標)
		float cameraDestY_;
		// 前のフレームは接地していたか否か
		bool wasGrounded_;
		// 今のフレームでは接地しているか否か
		bool isGrounded_;
		// 進行速度
		float lerpSpeed_;
		void UpdateLerpSpeed();
	};
} // namespace mtgb