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
		Vector3 lookAtPos_;
		Vector3 lookAtPositionOffset_;
		float rotateAngleDeg_;
		float distance_;
		InputType inputType_;

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
	};
} // namespace mtgb