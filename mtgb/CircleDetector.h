#pragma once

#include "IDetector.h"
#include "DetectorConfigs.h"
#include <functional>
#include "UIParams.h"
namespace mtgb
{
	/// <summary>
	/// 円形範囲内の対象検出システム
	/// </summary>
	class CircleDetector : public IDetector
	{
	  public:
		CircleDetector();
		CircleDetector(const CircleDetectorConfig& _config);
		CircleDetector(CircleDetectorConfig&& _config) noexcept;
		~CircleDetector() = default;

		/// <summary>
		/// 検出を実行
		/// </summary>
		void UpdateDetection() override;

		/// <summary>
		/// 検出を実行（設定指定）
		/// </summary>
		void UpdateDetection(const CircleDetectorConfig& _config);

		/// <summary>
		/// 設定を更新して検出を実行
		/// </summary>
		void UpdateAndSetDetection(CircleDetectorConfig&& _config);

		RectF GetDetectionArea() const override;

		/// <summary>
		/// 点が円内にあるかチェック
		/// </summary>
		bool IsPointInCircle(const Vector2F& _point, const Vector2F& _center, float _radius) const;

		CircleDetectorConfig config;
	};
} // namespace mtgb