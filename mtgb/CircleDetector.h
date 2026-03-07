#pragma once

#include "IDetector.h"
#include "DetectorConfigs.h"
#include <functional>
#include "UIDrawCommand.h"
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

		/// <summary>
		/// 検出された対象があるかどうか
		/// </summary>
		bool HasDetectedTargets() const override;

		RectF GetDetectionArea() const override;
		/// <summary>
		/// 検出結果を取得
		/// </summary>
		const std::vector<ScreenCoordContainsInfo>& GetDetectedTargets() const override;

		void ForEach(const std::function<void(ScreenCoordContainsInfo&)>& _func) override;
		void ForEach(const std::function<void(const ScreenCoordContainsInfo&)>& _func) const override;

		/// <summary>
		/// 点が円内にあるかチェック
		/// </summary>
		bool IsPointInCircle(const Vector2F& _point, const Vector2F& _center, float _radius) const;

		CircleDetectorConfig config;
	};
} // namespace mtgb