#pragma once

#include <vector>
#include <string>
#include <concepts>
#include "WindowContext/WindowContext.h"
#include "RectContainsInfo.h"
#include "Core/GameObject/GameObjectTag.h"
#include "Math/RectF.h"
namespace mtgb
{
	/// <summary>
	/// 検出システムのインターフェース
	/// </summary>
	class IDetector
	{
	  public:
		virtual ~IDetector() = default;

		/// <summary>
		/// 検出を実行
		/// </summary>
		virtual void UpdateDetection() = 0;

		/// <summary>
		/// 検出された対象があるかどうか
		/// </summary>
		/// <returns>対象が一つでもあるなら true</returns>
		bool HasDetectedTargets() const
		{
			return !detectedTargets_.empty();
		};

		/// <summary>
		/// 検出結果を取得
		/// </summary>
		/// <returns>検出された対象のリスト</returns>
		const std::vector<ScreenCoordContainsInfo>& GetDetectedTargets() const
		{
			return detectedTargets_;
		};

		/// <summary>
		/// 検出範囲を返す
		/// </summary>
		/// <returns></returns>
		virtual RectF GetDetectionArea() const
		{
			return RectF {};
		};
		/// <summary>
		/// 検出結果を操作
		/// </summary>
		template <typename Func>
			requires std::is_invocable_v<Func, ScreenCoordContainsInfo&>
		void ForEach(Func&& _func);
		template <typename Func>
			requires std::is_invocable_v<Func, const ScreenCoordContainsInfo&>
		void ForEach(Func&& _func) const;

	  protected:
		std::vector<ScreenCoordContainsInfo> detectedTargets_;
	};
	template <typename Func>
		requires std::is_invocable_v<Func, ScreenCoordContainsInfo&>
	inline void IDetector::ForEach(Func&& _func)
	{
		for (auto& target : detectedTargets_)
		{
			_func(target);
		}
	}
	template <typename Func>
		requires std::is_invocable_v<Func, const ScreenCoordContainsInfo&>
	inline void IDetector::ForEach(Func&& _func) const
	{
		for (const auto& target : detectedTargets_)
		{
			_func(target);
		}
	}
} // namespace mtgb