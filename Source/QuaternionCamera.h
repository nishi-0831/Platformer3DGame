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
		float currentLerpSpeed_;
		float lerpSpeedOnJumping_;

		float lerpSpeedOnDescending_;
		float lerpSpeedOnGrounded_;
		void UpdateLerpSpeed();
	};
} // namespace mtgb