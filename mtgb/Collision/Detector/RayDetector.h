#pragma once

#include "IDetector.h"
#include "DetectorConfigs.h"
#include <functional>

namespace mtgb
{
	/// <summary>
	/// レイ方向・角度による対象検出システム
	/// </summary>
	class RayDetector : public IDetector
	{
	  public:
		RayDetector() = default;
		RayDetector(const RayDetectorConfig& _config);
		RayDetector(RayDetectorConfig&& _config);
		~RayDetector() = default;

		/// <summary>
		/// 検出を実行
		/// </summary>
		void UpdateDetection() override;

		/// <summary>
		/// 検出を実行（設定指定）
		/// </summary>
		void UpdateDetection(const RayDetectorConfig& _config);

		/// <summary>
		/// 設定を更新して検出を実行
		/// </summary>
		void UpdateAndSetDetection(RayDetectorConfig&& _config);

		/// <summary>
		/// ターゲットがレイの角度範囲内にあるかチェック
		/// </summary>
		bool IsTargetInRayAngle(
			const Vector3& _targetPos,
			const Vector3& _rayOrigin,
			const Vector3& _rayDirection,
			float _maxAngleDegrees
		) const;

		RayDetectorConfig config;
	};
} // namespace mtgb