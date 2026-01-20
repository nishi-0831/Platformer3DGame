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
		Vector3 lookAtPos_;
		Vector3 lookAtPositionOffset_;
		float rotateAngleDeg_;
		float distance_;
		InputType inputType_;
		float minPitchAngleDeg_;
		float maxPitchAngleDeg_;

		float currentLerpSpeed_;
		float lerpSpeedOnJumping_;

		float lerpSpeedOnDescending_;
		float lerpSpeedOnGrounded_;
		void UpdateLerpSpeed();
	};
} // namespace mtgb